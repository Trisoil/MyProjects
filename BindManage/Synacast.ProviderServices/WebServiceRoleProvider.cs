using System;
using System.Collections.Generic;
using System.Text;
using System.Web.Security;

namespace Synacast.ProviderServices
{
    public class WebServiceRoleProvider:RoleProvider 
    {
        private RoleProviderService.PPRoleService service;

        private string _ApplicationName;
        private string _RemoteProviderName;

        public WebServiceRoleProvider()
        {
            service = new Synacast.ProviderServices.RoleProviderService.PPRoleService();    
        }

        public override void Initialize(string name, System.Collections.Specialized.NameValueCollection config)
        {
            if (!string.IsNullOrEmpty(config["roleProviderUri"]))
            {
                service.Url = config["roleProviderUri"];
            }
            if (!string.IsNullOrEmpty(config["applicationName"]))
            {
                _ApplicationName = config["applicationName"];
            }
            if (!string.IsNullOrEmpty(config["remoteProviderName"]))
            {
                _RemoteProviderName = config["remoteProviderName"];
            }
            base.Initialize(name, config);
        }

        public override string ApplicationName
        {
            get
            {
                return _ApplicationName;
            }
            set
            {
                _ApplicationName = value;
            }
        }

        public override void AddUsersToRoles(string[] usernames, string[] roleNames)
        {
            service.AddUsersToRoles(_RemoteProviderName, _ApplicationName, usernames, roleNames);
        }

        public override void CreateRole(string roleName)
        {
            service.CreateRole(_RemoteProviderName, _ApplicationName, roleName);
        }

        public override bool DeleteRole(string roleName, bool throwOnPopulatedRole)
        {
            return service.DeleteRole(_RemoteProviderName, _ApplicationName, roleName, throwOnPopulatedRole);
        }

        public override string[] FindUsersInRole(string roleName, string usernameToMatch)
        {
            return service.FindUsersInRole(_RemoteProviderName, _ApplicationName, roleName, usernameToMatch);
        }

        public override string[] GetAllRoles()
        {
            return service.GetAllRoles(_RemoteProviderName, _ApplicationName);
        }

        public override string[] GetRolesForUser(string username)
        {
            return service.GetRolesForUser(_RemoteProviderName, _ApplicationName, username);
        }

        public override string[] GetUsersInRole(string roleName)
        {
            return service.GetUsersInRole(_RemoteProviderName, _ApplicationName, roleName);
        }

        public override bool IsUserInRole(string username, string roleName)
        {
            return service.IsUserInRole(_RemoteProviderName, _ApplicationName, username, roleName);
        }

        public override void RemoveUsersFromRoles(string[] usernames, string[] roleNames)
        {
            service.RemoveUsersFromRoles(_RemoteProviderName, _ApplicationName, usernames, roleNames);
        }

        public override bool RoleExists(string roleName)
        {
            return service.RoleExists(_RemoteProviderName, _ApplicationName, roleName);
        }
    }
}
