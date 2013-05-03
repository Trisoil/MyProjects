#ifndef _TESTRESULTREPORTER_H_
#define _TESTRESULTREPORTER_H_

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
    class TestResultReporter : public EmptyTestEventListener, public ReportBase
    {
    public:
        TestResultReporter(char* path):ReportBase(path)
        {

        }
    private:
        struct _tResult
        {
            _tResult()
                : resultBuild( "N/A" )
                , resultStatus( "Completed" )
                , assignedTo( "Closed" )
                , bugSeverity( "3" )
                , bugType( "CodeError" )
            {
            }

            std::string projectTitle;
            std::string modulePath;
            std::string caseTitle;
            std::string resultValue;
            std::string resultBuild;
            std::string resultStatus;
            std::string assignedTo;
            std::string replyNote;
            std::string bugSeverity;
            std::string bugType;
        };

    private:
        void writeXMLResult( FILE * fp, _tResult const & rt )
        {
            std::ostringstream stream;
            stream << "\t<result ProjectTitle=\"" << project_name()
                << "\" ModulePath=\"" << rt.modulePath.c_str()
                << "\" CaseTitle=\"" << rt.caseTitle.c_str()
                << "\" ResultValue=\"" << rt.resultValue.c_str()
                << "\" ResultBuild=\"" << rt.resultBuild.c_str()
                << "\" ResultStatus=\"" << rt.resultStatus.c_str()
                << "\" AssignedTo=\"" << rt.assignedTo.c_str()
                << "\" ReplyNote=\"" << rt.replyNote.c_str()
                << "\" BugSeverity=\"" << rt.bugSeverity.c_str()
                << "\" BugType=\"" << rt.bugType.c_str()
                << "\" />\n";
            std::string dest;
            trans_encode(dest, &stream.str());
            fprintf(fp, dest.c_str());
        }

        int replace_all( std::string & str, const std::string & pattern, const std::string & newpat )
        { 
            int count = 0; 
            const size_t nsize = newpat.size();
            const size_t psize = pattern.size();

            for( size_t pos = str.find( pattern, 0 );
                pos != std::string::npos;
                pos = str.find( pattern,pos + nsize ) )
            { 
                str.replace( pos, psize, newpat ); 
                count++; 
            } 

            return count; 
        }

        virtual void OnTestProgramEnd( const UnitTest & unit_test ) 
        {
            FILE *fp = fopen( export_path("results.xml"), "w" );

            writeXMLHeader( fp , "results" );
            
            for ( int i = 0; i < unit_test.total_test_case_count(); ++i )
            {
                const TestCase * test_case = unit_test.GetTestCase( i );

                for (int j = 0; j < test_case->total_test_count(); j++) 
                {
                    _tResult rt;
                    
                    const TestInfo* const test_info = test_case->GetTestInfo( j );

                    const TestResult * test_result = test_info->result();
                    for ( int k = 0; k < test_result->test_property_count(); ++k )
                    {
                        const TestProperty & test_prop = test_result->GetTestProperty( k );
                        if ( !strcmp( test_prop.key(), "Version" ) ) 
                            rt.resultBuild = test_prop.value();
                    }

                    for ( int k = 0; k < test_result->total_part_count(); ++k )
                    {
                        const TestPartResult & test_part = test_result->GetTestPartResult( k );
                        rt.replyNote += "Filename: ";
                        rt.replyNote += test_part.file_name();
                        rt.replyNote += "\nLine number: ";
                        char buf[32] = { 0 };
                        sprintf( buf, "%d", test_part.line_number() );
                        rt.replyNote += buf;
                        //rt.replyNote += "\nSummary: ";
                        //rt.replyNote += test_part.summary();
                        rt.replyNote += "\nMessage: ";
                        rt.replyNote += test_part.message();
                        replace_all( rt.replyNote, "\\", "\\\\" );
                        replace_all( rt.replyNote, "\"", "-" );

                        if ( test_part.fatally_failed() )
                        {
                            rt.resultStatus = "1";
                            rt.bugSeverity = "1";
                        }
                    }
                    if(!test_result->Passed())
                    {
                        const TestModule* test_module = test_info->module();
                        if(test_module)
                        {
                            rt.assignedTo = test_module->m_CoderName_;
                            if(rt.bugSeverity != "1")
                            {
                                rt.bugSeverity = test_module->m_Level_;
                            }
                        }
                    }
                    rt.resultValue = test_result->Passed() ? "Pass" : "Fail";
                    rt.modulePath = test_case->name();
                    module_path(rt.modulePath);
                    rt.caseTitle = test_case->name();
                    rt.caseTitle += ".";
                    rt.caseTitle += test_info->name();
                    std::string t_name = test_info->name();
                    case_type(rt.bugType, &t_name);
                    if(rt.bugType == "Functional")
                    {
                        rt.bugType = "CodeError";
                    }
                    writeXMLResult( fp, rt );
                }
            }
            writeXMLTail( fp );
            fclose( fp );
        }
    };
}
#endif
