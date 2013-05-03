using System;
using System.Collections.Generic;
using System.Text;
using System.Web.Security;

namespace Synacast.ProviderServices
{
    public class WebServiceMembershipProvider : MembershipProvider
    {
        private MembershipProviderService.PPMembershipService service;
        private bool _EnablePasswordRetrieval;
        private bool _EnablePasswordReset;

        private string _ApplicationName;
        private string _RemoteProviderName;

        public WebServiceMembershipProvider()
        {
            service = new Synacast.ProviderServices.MembershipProviderService.PPMembershipService();
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

        public override void Initialize(string name, System.Collections.Specialized.NameValueCollection config)
        {
            if (config["roleProviderUri"] != null)
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
            _EnablePasswordRetrieval = ProviderUtility.GetBooleanValue(config, "enablePasswordRetrieval", true);
            _EnablePasswordReset = ProviderUtility.GetBooleanValue(config, "enablePasswordReset", true);
            base.Initialize(name, config);
        }

        public override bool ChangePassword(string username, string oldPassword, string newPassword)
        {
            return service.ChangePassword(_RemoteProviderName, _ApplicationName, username, oldPassword, newPassword);
        }

        public override bool ChangePasswordQuestionAndAnswer(string username, string password, string newPasswordQuestion, string newPasswordAnswer)
        {
            return service.ChangePasswordQuestionAndAnswer(_RemoteProviderName, _ApplicationName, username, password, newPasswordQuestion, newPasswordAnswer);
        }

        public override MembershipUser CreateUser(string username, string password, string email, string passwordQuestion, string passwordAnswer, bool isApproved, object providerUserKey, out MembershipCreateStatus status)
        {
            throw new Exception("The method or operation is not implemented.");
        }

        public override bool DeleteUser(string username, bool deleteAllRelatedData)
        {
            return service.DeleteUser(_RemoteProviderName, _ApplicationName, username, deleteAllRelatedData);
        }

        public override bool EnablePasswordReset
        {
            get { return _EnablePasswordReset; }
        }

        public override bool EnablePasswordRetrieval
        {
            get { return _EnablePasswordRetrieval; }
        }

        public override MembershipUserCollection FindUsersByEmail(string emailToMatch, int pageIndex, int pageSize, out int totalRecords)
        {
            throw new Exception("The method or operation is not implemented.");
        }

        public override MembershipUserCollection FindUsersByName(string usernameToMatch, int pageIndex, int pageSize, out int totalRecords)
        {
            throw new Exception("The method or operation is not implemented.");
        }

        public override MembershipUserCollection GetAllUsers(int pageIndex, int pageSize, out int totalRecords)
        {
            throw new Exception("The method or operation is not implemented.");
        }

        public override int GetNumberOfUsersOnline()
        {
            return service.GetNumberOfUsersOnline(_RemoteProviderName, _ApplicationName);
        }

        public override string GetPassword(string username, string answer)
        {
            return service.GetPassword(_RemoteProviderName, _ApplicationName, username, answer);
        }

        public override MembershipUser GetUser(string username, bool userIsOnline)
        {
            throw new Exception("The method or operation is not implemented.");
        }

        public override MembershipUser GetUser(object providerUserKey, bool userIsOnline)
        {
            throw new Exception("The method or operation is not implemented.");
        }

        public override string GetUserNameByEmail(string email)
        {
            throw new Exception("The method or operation is not implemented.");
        }

        public override int MaxInvalidPasswordAttempts
        {
            get { throw new Exception("The method or operation is not implemented."); }
        }

        public override int MinRequiredNonAlphanumericCharacters
        {
            get { throw new Exception("The method or operation is not implemented."); }
        }

        public override int MinRequiredPasswordLength
        {
            get { throw new Exception("The method or operation is not implemented."); }
        }

        public override int PasswordAttemptWindow
        {
            get { throw new Exception("The method or operation is not implemented."); }
        }

        public override MembershipPasswordFormat PasswordFormat
        {
            get { throw new Exception("The method or operation is not implemented."); }
        }

        public override string PasswordStrengthRegularExpression
        {
            get { throw new Exception("The method or operation is not implemented."); }
        }

        public override bool RequiresQuestionAndAnswer
        {
            get { throw new Exception("The method or operation is not implemented."); }
        }

        public override bool RequiresUniqueEmail
        {
            get { throw new Exception("The method or operation is not implemented."); }
        }

        public override string ResetPassword(string username, string answer)
        {
            throw new Exception("The method or operation is not implemented.");
        }

        public override bool UnlockUser(string userName)
        {
            throw new Exception("The method or operation is not implemented.");
        }

        public override void UpdateUser(MembershipUser user)
        {
            throw new Exception("The method or operation is not implemented.");
        }

        public override bool ValidateUser(string username, string password)
        {
            return service.ValidateUser(_RemoteProviderName, _ApplicationName, username, password);
        }

        private System.Web.Security.MembershipUserCollection BuildUserCollection(MembershipUser[] list)
        {
            if (list == null) return null;
            System.Web.Security.MembershipUserCollection collection = new System.Web.Security.MembershipUserCollection();
            foreach (MembershipUser user in list)
            {
                collection.Add(user);
            }
            return collection;
        }
    }
}
