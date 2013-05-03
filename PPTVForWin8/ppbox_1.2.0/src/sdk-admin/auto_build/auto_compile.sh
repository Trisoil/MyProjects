#!/bin/bash

function error_msg()
{
	sData=`date`
	echo "[ERROR] <${sData}> : ${1}"
	exit 1
}

function success_msg()
{
	sData=`date`
	echo "[SUCCESS] <${sData}> : ${1}"
}

function file_is_exist()
{
	if [ ! -e "${1}" ];then
		error_msg "${1} is not exist"
	fi
}

function dir_is_exist()
{
	if [ ! -d "${1}" ];then
		error_msg "${1} is not exist"
	fi			 
}

function complile_base()
{
	success_msg "Enter complile_base" 
	base_build_dir="${base_rootdir}/${1}/build"
	success_msg "base_build_dir: ${base_build_dir}"
	dir_is_exist ${base_build_dir}
	cd ${base_build_dir}
	success_msg "make config=release ${platform} all-project"
	make config=release ${platform} all-project
	success_msg "make config=release all-project publish"
	make config=release all-project publish
}

function update_base()
{
	success_msg "Enter update_base"
	version_file="${box_rootdir}/${1}/include/base/framework/Define.h"
	file_is_exist ${version_file}
	version_base=`grep "VERSION" ${version_file} | awk 'BEGIN{RS="\r\n"}{print $3}'`

	version_base=`echo $version_base`	
	basedir="${base_rootdir}/${version_base}"
	success_msg "basedir: ${basedir}"
	complile_base ${version_base}
}

function update_ppbox()
{
	success_msg "Enter update_ppbox"
	boxdir="${box_rootdir}/${1}"
	success_msg "boxdir: ${boxdir}"
	dir_is_exist ${boxdir}
	cd ${boxdir}
	svn up
}


#file dir
function move_file()
{
	success_msg "Enter move_file"
	full_path=${1}
	success_msg "cp ${full_path} ${out_dir}"
	cp ${full_path} ${out_dir}
	
	paket_name=`echo "${full_path}" | awk 'BEGIN{FS="/"}{print $NF}'`
	echo "${paket_name}"
}

# packet/ppbox dir
function filename()
{
	success_msg "Enter filename" 
	packet_dir=${1}
	
	dir_is_exist ${packet_dir}
	cd ${packet_dir}
	
	filename=`make config=release info | awk 'BEGIN{FS=":"}{if("File" == $1) print $2}'`
	filename=`echo $filename`
	success_msg "filename:${filename}"
	
	file_is_exist ${filename}
	move_file "${packet_dir}/${filename}"	
}



# platform version strategy
function complile_ppbox()
{
	success_msg "Enter complile_ppbox"
	update_ppbox ${2}
	success_msg "Enter complile"
	build_dir="${box_rootdir}/${2}/build"
	full_platform_name="${1}.${3}"
	success_msg "full_platform_name: ${full_platform_name}"		
	cd ${build_dir}
	success_msg "make config=release ${full_platform_name} packet/ppbox"
	make config=release ${full_platform_name} packet/ppbox
	filename "${build_dir}/${full_platform_name}/packet/ppbox"
}

function complile()
{
	update_base ${2}
#	complile_base ${1} ${2} ${3}
	complile_ppbox ${1} ${2} ${3}
}


#当前运行目录
box_rootdir=/home/zenzhang/svn_code/publish
base_rootdir=/home/zenzhang/svn_code/publish

#发布目录
out_dir=/var/www/packet/sdk

if [ $# != 3 ];then
	error_msg "worng params,For example: ./a.sh linux-x86 1.0.0 httpd"
fi

platform=${1}
version=${2}
strategy=${3}

complile ${platform} ${version} ${strategy}

success_msg "Finish Complile"
exit 0
