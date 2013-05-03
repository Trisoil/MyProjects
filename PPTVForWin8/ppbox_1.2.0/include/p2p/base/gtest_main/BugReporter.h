#ifndef _BUGREPORTER_H_
#define _BUGREPORTER_H_

#include <stdio.h>
#include <sstream>
#include "gtest/gtest.h"
#include "gtest_main/ReportBase.h"

using ::testing::EmptyTestEventListener;
using ::testing::InitGoogleTest;
using ::testing::Test;
using ::testing::TestCase;
using ::testing::TestEventListeners;
using ::testing::TestInfo;
using ::testing::TestProperty;
using ::testing::TestPartResult;
using ::testing::TestResult;
using ::testing::UnitTest;

namespace gtestframe
{
    class BugReporter : public EmptyTestEventListener , public ReportBase 
    {
    public:
        BugReporter(char* path):ReportBase(path)
        {

        }
    private:
        struct _tBug
        {
            _tBug()
                : assignedTo( "Active" )
                , bugSeverity( "3" )
                , bugType( "CodeError" )
                , howFound( "FuncTest" )
                , openedBuild( "N/A" )
                , bugPriority( "3" )
            {

            }

            std::string projectTitle;
            std::string modulePath;
            std::string bugTitle;
            std::string reproSteps;
            std::string assignedTo;
            std::string bugSeverity;
            std::string bugType;
            std::string howFound;
            std::string openedBuild;
            std::string replyNote;
            std::string bugKeyword;
            std::string bugPriority;
            std::string mailTo;
        };
    private:
        void writeXMLBug( FILE * fp, _tBug const & bg )
        {
            std::ostringstream stream;
            stream << "\t<bug ProjectTitle=\"" << project_name()
                << "\" ModulePath=\"" << bg.modulePath.c_str()
                << "\" BugTitle=\"" << bg.bugTitle.c_str()
                << "\" ReproSteps=\"" << bg.reproSteps.c_str()
                << "\" AssignedTo=\"" << bg.assignedTo.c_str()
                << "\" BugSeverity=\"" << bg.bugSeverity.c_str()
                << "\" BugType=\"" << bg.bugType.c_str()
                << "\" HowFound=\"" << bg.howFound.c_str()
                << "\" OpenedBuild=\"" << bg.openedBuild.c_str()
                << "\" ReplyNote=\"" << bg.replyNote.c_str()
                << "\" BugKeyword=\"" << bg.bugKeyword.c_str()
                << "\" BugPriority=\"" << bg.bugPriority.c_str()
                << "\" MailTo=\"" << bg.mailTo.c_str()
                << "\" />\n";
            std::string dest;
            trans_encode(dest, &stream.str());
            fprintf(fp,dest.c_str());
        }

        virtual void OnTestProgramEnd( const UnitTest & unit_test ) 
        {
            FILE *fp = fopen( export_path("bugs.xml"), "w" );
            writeXMLHeader( fp ,"bugs");
            for ( int i = 0; i < unit_test.total_test_case_count(); ++i )
            {// 获取每一个测试案例
                const TestCase * test_case = unit_test.GetTestCase( i );

                for (int j = 0; j < test_case->total_test_count(); j++) 
                {// 获取每一个测试
                    const TestInfo* const test_info = test_case->GetTestInfo( j );
                    if ( test_info->result()->Failed() )
                    {
                        _tBug bg;
                        const TestResult * test_result = test_info->result();
                        for ( int k = 0; k < test_result->test_property_count(); ++k )
                        {
                            const TestProperty & test_prop = test_result->GetTestProperty( k );
                            /*if ( !strcmp( test_prop.key(), "ProjectTitle" ) ) 
                                bg.projectTitle = test_prop.value();
                            if ( !strcmp( test_prop.key(), "ModulePath" ) ) 
                                bg.modulePath = test_prop.value();*/
                            if ( !strcmp( test_prop.key(), "Version" ) ) 
                                bg.openedBuild = test_prop.value();
                        }

                        for ( int k = 0; k < test_result->total_part_count(); ++k )
                        {
                            const TestPartResult & test_part = test_result->GetTestPartResult( k );
                            bg.replyNote += "Filename: ";
                            bg.replyNote += test_part.file_name();
                            bg.replyNote += "\nLine number: ";
                            char buf[32] = { 0 };
                            sprintf( buf, "%d", test_part.line_number() );
                            bg.replyNote += buf;
                            bg.replyNote += "\nSummary: ";
                            bg.replyNote += test_part.summary();
                            bg.replyNote += "\nMessage: ";
                            bg.replyNote += test_part.message();
                            replace_all( bg.replyNote, "\\", "\\\\" );
                            replace_all( bg.replyNote, "\"", "-" );

                            if ( test_part.fatally_failed() )
                            {
                                bg.bugPriority = "1";
                                bg.bugSeverity = "1";
                            }
                        }

                        const TestModule * test_module = test_info->module();
                        if(test_module)
                        {
                            bg.assignedTo = test_module->m_CoderName_;
                            if(bg.bugPriority != "1")
                            {
                                bg.bugPriority = test_module->m_Level_;
                                bg.bugSeverity = test_module->m_Level_;
                            }
                        }
                        bg.modulePath = test_case->name();
                        module_path(bg.modulePath);
                        bg.bugTitle = test_case->name();
                        bg.bugTitle += ".";
                        bg.bugTitle += test_info->name();
                        writeXMLBug( fp, bg );
                    }
                }
            }
            writeXMLTail( fp );
            fclose( fp );
        }
    };
}
#endif
