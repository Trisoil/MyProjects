//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#include "Common.h"
#include "DiskTask.h"
#include "SmallFileLocalDisk.h"

namespace super_node
{
    ErrorCode DiskWalkTask::Execute(boost::shared_ptr<SmallFileLocalDisk> disk)
    {
        std::vector<boost::shared_ptr<DiskResource> > resources;
        EnumerateResources(disk, resources);
        if (callback_)
        {
            callback_(ErrorCodes::Success, resources);
        }

        return ErrorCodes::Success;
    }

    void DiskWalkTask::EnumerateResources(boost::shared_ptr<SmallFileLocalDisk> disk, std::vector<boost::shared_ptr<DiskResource> >& resources)
    {
        boost::filesystem::path disk_path = disk->GetDiskPath();

        LOG4CPLUS_TRACE_METHOD(Loggers::LocalDisk(), "DiskWalkTask::EnumerateResources()");

        DoEnumerateResources(disk, resources);

        disk->UpdateResources(resources);
    }

    void DiskWalkTask::DoEnumerateResources(boost::shared_ptr<SmallFileLocalDisk> disk, std::vector<boost::shared_ptr<DiskResource> >& actual_resources)
    {
        boost::filesystem::path disk_path = disk->GetDiskPath();

        LOG4CPLUS_TRACE_METHOD(Loggers::LocalDisk(), "DiskWalkTask::DoEnumerateResources()");

        const boost::filesystem::directory_iterator end_iter;
        //遍历第一级目录:resource_name的MD5的[0,1]位
        for (boost::filesystem::directory_iterator level1_dir_iter(disk_path); 
            level1_dir_iter != end_iter; 
            ++level1_dir_iter) 
        {
            const int IntermediateFolderNameSize = 2;
            std::string level1_name = ((boost::filesystem::path)(*level1_dir_iter)).leaf();
            if (!boost::filesystem::is_directory(*level1_dir_iter) ||
                level1_name.size() != IntermediateFolderNameSize) 
            {
                continue;
            }

            //遍历第二级目录:resource_name的MD5的[2,3]位
            for (boost::filesystem::directory_iterator level2_dir_iter(disk_path / level1_name); 
                 level2_dir_iter != end_iter; 
                 ++level2_dir_iter) 
            {
                boost::filesystem::path level2_dir_path = (boost::filesystem::path)(*level2_dir_iter);
                std::string level2_name = level2_dir_path.leaf();
                if (!boost::filesystem::is_directory(*level2_dir_iter) || 
                    level2_name.size() != IntermediateFolderNameSize) 
                {
                    continue;
                }
                
                //第三级目录: resource_name即目录名本身
                for (boost::filesystem::directory_iterator resource_dir_iter(level2_dir_path); 
                    resource_dir_iter != end_iter; 
                    ++resource_dir_iter) 
                {
                    boost::filesystem::path resource_directory_path = (boost::filesystem::path)(*resource_dir_iter);
                    std::string resource_name = resource_directory_path.leaf();
                    if (!boost::filesystem::is_directory(*resource_dir_iter)) 
                    {
                        continue;
                    }
                    
                    ResourceIdentifier resource_identifier(resource_name);

                    boost::shared_ptr<DiskResource> disk_resource = LoadResource(disk, resource_identifier, resource_directory_path);

                    if (disk_resource)
                    {
                        actual_resources.push_back(disk_resource);
                    }
                    else if (delete_resource_on_error_)
                    {
                        RemoveFromDisk(resource_directory_path);
                        LOG4CPLUS_INFO(Loggers::LocalDisk(), "Detect incorrect blocks, already remove whole resource path :" << resource_directory_path.string());
                    }
                    else 
                    {
                        LOG4CPLUS_WARN(Loggers::LocalDisk(), "Detect incorrect blocks,  path :" << resource_directory_path.string());
                    }
                }
            }
        }
    }

    boost::shared_ptr<DiskResource>  DiskWalkTask::LoadResource(
        boost::shared_ptr<SmallFileLocalDisk> disk, 
        const ResourceIdentifier& resource_identifier, 
        const boost::filesystem::path& resource_directory_path)
    {
        BlocksMap blocks_map;
        boost::shared_ptr<DiskResource> disk_resource(new DiskResource(resource_identifier, false, blocks_map));
        
        boost::filesystem::path expected_resource_directory_path = disk->GetResourceFolderPath(resource_identifier);
        
        LOG4CPLUS_TRACE(Loggers::LocalDisk(), "DiskWalkTask::VerifyResource() - " << resource_directory_path);

        if (expected_resource_directory_path != resource_directory_path)
        {
            LOG4CPLUS_WARN(Loggers::LocalDisk(), "DiskWalkTask::VerifyResource() - resource folder " << resource_directory_path <<" is invalid and will be ignored.");
            return boost::shared_ptr<DiskResource>();
        }

        int max_block_index = -1;
        int incomplete_block_index = -1;
        int blocks_count = 0;

        const boost::filesystem::directory_iterator end_iter; 
        for (boost::filesystem::directory_iterator dir_iter(resource_directory_path); 
             dir_iter != end_iter; 
             ++dir_iter) 
        {
            if (boost::filesystem::is_regular_file(*dir_iter)) 
            {
                string file_name = ((boost::filesystem::path)(*dir_iter)).leaf();
                
                //xxx.block
                const size_t BlockFileNameSize = 9;
                const string BlockFileNameSubfix = ".block";
                if (file_name.size() == BlockFileNameSize && file_name.find(BlockFileNameSubfix) == 3)
                {
                    if (file_name[0] >= '0' && file_name[0] <= '9' && 
                        file_name[1] >= '0' && file_name[1] <= '9' &&
                        file_name[2] >= '0' && file_name[2] <= '9')
                    {
                        int block_index = (file_name[0]  - '0') * 100 + (file_name[1] - '0') * 10 + (file_name[2] - '0');
                        if (block_index > max_block_index)
                        {
                            max_block_index = block_index;
                        }

                        boost::uintmax_t block_file_size = boost::filesystem::file_size(*dir_iter);
                        if (block_file_size > BlockData::MaxBlockSize)
                        {
                            return boost::shared_ptr<DiskResource>();
                        }

                        if (block_file_size != BlockData::MaxBlockSize)
                        {
                            if (incomplete_block_index >= 0)
                            {
                                //最多只有最后一个block是不完整的
                                return boost::shared_ptr<DiskResource>();
                            }

                            incomplete_block_index = block_index;
                        }

                        disk_resource->AddBlock(static_cast<size_t>(block_index));
                        ++blocks_count;
                    }
                }
            }
        }
        
        if (blocks_count == 0 ||
            // if there is one incomplete block but no the last one
            (incomplete_block_index >= 0 && incomplete_block_index != max_block_index) ||  
            //if there is one incomplete block and is not 000.block
            (blocks_count == 1 && incomplete_block_index > 0 && delete_resource_on_error_))        
        {
            return boost::shared_ptr<DiskResource>(); 
        }

        return disk_resource;
    }

    bool DiskWalkTask::RemoveFromDisk(const boost::filesystem::path & path)
    {
        try
        {
            if (boost::filesystem::exists(path))
            {
                boost::filesystem::remove_all(path);
            }
            else
            {
                assert(false);
            }
        }
        catch (const boost::filesystem::filesystem_error& error)
        {
            LOG4CPLUS_ERROR(Loggers::LocalDisk(), "An error occurred while trying to remove path " << path << ". Error:" << error.what());
            return false;
        }

        return true;
    }


    void DiskWalkTask::Cancel()
    {
        if (callback_)
        {
            std::vector<boost::shared_ptr<DiskResource> > temp;
            callback_(ErrorCodes::ServiceBusy, temp);
        }
    }
}
