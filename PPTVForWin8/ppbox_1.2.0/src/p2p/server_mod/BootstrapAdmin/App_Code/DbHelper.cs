using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;
using Mis.Data;
using System.Data.SqlClient;
using MySql.Data.MySqlClient;

/// <summary>
///DbHelper 的摘要说明
/// </summary>
public static class DbHelper
{

    public static IDbHelper BootStrapServers { get; private set; }

    //public static IDbHelper OpenService_CDNCooperators { get; private set; }

    public static IDbHelper OpenService_Files { get; private set; }

	static DbHelper()
	{
        BootStrapServers = new DbHelper<SqlConnection, SqlCommand, SqlParameter>();
        BootStrapServers.ConnectionString = System.Configuration.ConfigurationManager.ConnectionStrings["BootStrapServers"].ConnectionString;

        //OpenService_CDNCooperators = new DbHelper<MySqlConnection, MySqlCommand, MySqlParameter>();
        //OpenService_CDNCooperators.ConnectionString = System.Configuration.ConfigurationManager.ConnectionStrings["OpenService_CDNCooperators"].ConnectionString;

        OpenService_Files = new DbHelper<MySqlConnection, MySqlCommand, MySqlParameter>();
        OpenService_Files.ConnectionString = System.Configuration.ConfigurationManager.ConnectionStrings["OpenService_Files"].ConnectionString;
    }
}
