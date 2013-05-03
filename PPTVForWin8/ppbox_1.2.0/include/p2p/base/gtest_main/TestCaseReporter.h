#ifndef _TESTCASEREPORTER_H_
#define _TESTCASEREPORTER_H_

#include <stdio.h>
#include <sstream>
#include "gtest/gtest.h"
#include "gtest_main/ReportBase.h"

using ::testing::EmptyTestEventListener;
using ::testing::InitGoogleTest;
using ::testing::Test;
using ::testing::TestCase;
using ::testing::TestEventListeners;
using ::testing::TestModule;
using ::testing::TestInfo;
using ::testing::TestProperty;
using ::testing::TestPartResult;
using ::testing::TestResult;
using ::testing::UnitTest;

namespace gtestframe
{
    class TestCaseReporter : public EmptyTestEventListener , public ReportBase
    {
    public:
        TestCaseReporter(char* path):ReportBase(path)
        {

        }
    private:
        struct _tCase
        {
            _tCase()
                : caseStatus( "Active" )
                , caseType( "Functional" )
                , assignedTo( "Active" )
                , casePriority( "3" )
                , caseMethod( "Automation" )
                , scriptStatus( "NotPlanned" )
                , casePlan( "Function" )
            {

            }

            std::string projectTitle;
            std::string modulePath;
            std::string caseTitle;
            std::string caseSteps;
            std::string caseStatus;
            std::string caseType;
            std::string assignedTo;
            std::string casePriority;
            std::string caseMethod;
            std::string replyNote;
            std::string scriptedBy;
            std::string scriptedDate;
            std::string scriptStatus;
            std::string scriptLocation;
            std::string casePlan;
            std::string caseKeyword;
            std::string mailTo;
        };
    private:
        void writeXMLCase( FILE * fp, _tCase const & cs )
        {
            std::ostringstream stream;
            stream << "\t<case ProjectTitle=\"" << project_name()
                << "\" ModulePath=\"" << cs.modulePath.c_str()
                << "\" CaseTitle=\"" << cs.caseTitle.c_str()
                << "\" CaseSteps=\"" << cs.caseSteps.c_str()
                << "\" CaseStatus=\"" << cs.caseStatus.c_str()
                << "\" CaseType=\"" << cs.caseType.c_str()
                << "\" AssignedTo=\"" << cs.assignedTo.c_str()
                << "\" CasePriority=\"" << cs.casePriority.c_str()
                << "\" CaseMethod=\"" << cs.caseMethod.c_str()
                << "\" ReplyNote=\"" << cs.replyNote.c_str()
                << "\" ScriptedBy=\"" << cs.scriptedBy.c_str()
                << "\" ScriptedDate=\"" << cs.scriptedDate.c_str()
                << "\" ScriptStatus=\"" << cs.scriptStatus.c_str()
                << "\" ScriptLocation=\"" << cs.scriptLocation.c_str()
                << "\" CasePlan=\"" << cs.casePlan.c_str()
                << "\" CaseKeyword=\"" << cs.caseKeyword.c_str()
                << "\" MailTo=\"" << cs.mailTo.c_str()
                << "\" />\n";
            std::string dest;
            trans_encode( dest, &stream.str() );
            fprintf( fp, dest.c_str() );
        }

        virtual void OnTestProgramEnd( const UnitTest & unit_test ) 
        {
            FILE *fp = fopen( export_path("cases.xml"), "w" );
            writeXMLHeader( fp , "cases" );
            for ( int i = 0; i < unit_test.total_test_case_count(); i++ )
            {
                const TestCase * test_case = unit_test.GetTestCase( i );

                for (int j = 0; j < test_case -> total_test_count(); j++) 
                {
                    const TestInfo* const test_info = test_case -> GetTestInfo( j );
                    _tCase cs;

                    const TestResult * test_result = test_info -> result();
                    /*for ( int k = 0; k < test_result->test_property_count(); k++ )
                    {
                        const TestProperty & test_prop = test_result->GetTestProperty( k );
                        if ( !strcmp( test_prop.key(), "ProjectTitle" ) ) 
                            cs.projectTitle = test_prop.value();
                        if ( !strcmp( test_prop.key(), "ModulePath" ) ) 
                            cs.modulePath = test_prop.value();
                    }*/

                    const TestModule * test_module = test_info->module();
                    if ( test_module )
                    {
                        if(!test_result->Passed())
                            cs.assignedTo = test_module -> m_CoderName_;
                        cs.scriptedBy = test_module -> m_TesterName_;
                        cs.casePriority = test_module -> m_Level_;
                    }
                    cs.modulePath = test_case -> name();
                    module_path(cs.modulePath);
                    cs.caseTitle = test_case -> name();
                    cs.caseTitle += ".";
                    cs.caseTitle += test_info -> name();
                    std::string t_name = test_info->name();
                    case_type(cs.caseType, &t_name);
                    writeXMLCase( fp, cs );
                }
            }
            writeXMLTail( fp );
            fclose( fp );
        }
    };
}
#endif
