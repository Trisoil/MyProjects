using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Diagnostics;

using Windows.ApplicationModel;
using Windows.ApplicationModel.Activation;
using Windows.Foundation;
using Windows.Foundation.Collections;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Controls.Primitives;
using Windows.UI.Xaml.Data;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Navigation;
using Windows.UI.ApplicationSettings;
using Windows.ApplicationModel.Search;
using Windows.ApplicationModel.DataTransfer;
using Windows.Storage.Streams;
using Windows.UI.Popups;

// The Grid Application template is documented at http://go.microsoft.com/fwlink/?LinkId=234226

namespace PPTVForWin8
{
    using PPTVData.Utils;
    using PPTVData.Entity;
    using PPTVData.Factory;
    using PPTVData.Factory.WAY;

    using PPTVForWin8.Common;
    using PPTVForWin8.Utils;
    using PPTVForWin8.Pages;

    /// <summary>
    /// Provides application-specific behavior to supplement the default Application class.
    /// An overview of the Grid Application design will be linked to in future revisions of
    /// this template.
    /// </summary>
    sealed partial class App : Application
    {
        /// <summary>
        /// Initializes the singleton application object.  This is the first line of authored code
        /// executed, and as such is the logical equivalent of main() or WinMain().
        /// </summary>
        public App()
        {
            this.InitializeComponent();

            this.Resuming += App_Resuming;
            this.Suspending += App_Suspending;
            this.UnhandledException += App_UnhandledException;

            PPTVData.Utils.DataCommonUtils.AppStartTime = DateTime.Now;
        }

        /// <summary>
        /// Invoked when application execution is being suspended.  Application state is saved
        /// without knowing whether the application will be terminated or resumed with the contents
        /// of memory still intact.
        /// </summary>
        /// <param name="sender">The source of the suspend request.</param>
        /// <param name="e">Details about the suspend request.</param>
        void App_Suspending(object sender, SuspendingEventArgs e)
        {
            var deferral = e.SuspendingOperation.GetDeferral();
            PagesUtils.ChangePlayState(MediaElementState.Paused);
            DACFactory.Instance.Quit();
            deferral.Complete();
        }

        void App_Resuming(object sender, object e)
        {
            DACFactory.Instance.StartUp();
            PagesUtils.ChangePlayState(MediaElementState.Playing);
        }

        void App_UnhandledException(object sender, UnhandledExceptionEventArgs e)
        {
            e.Handled = true;
            CommonUtils.ShowMessage(e.Message);
        }

        /// <summary>
        /// Invoked when the application is launched normally by the end user.  Other entry points
        /// will be used when the application is launched to open a specific file, to display
        /// search results, and so forth.
        /// </summary>
        /// <param name="args">Details about the launch request and process.</param>
        protected override void OnLaunched(LaunchActivatedEventArgs args)
        {
            if (args.PreviousExecutionState == ApplicationExecutionState.Running || 
                args.PreviousExecutionState == ApplicationExecutionState.Suspended)
            {
                if (args.PreviousExecutionState == ApplicationExecutionState.Suspended)
                    DACFactory.Instance.StartUp();

                SecondaryTileNavgate(args.Arguments);
            }
            else
            {
                FirstLaunch(args);
            }
        }

        protected override void OnSearchActivated(SearchActivatedEventArgs args)
        {
            if (args.PreviousExecutionState == ApplicationExecutionState.Running || 
                args.PreviousExecutionState == ApplicationExecutionState.Suspended)
            {
                if (args.PreviousExecutionState == ApplicationExecutionState.Suspended)
                    DACFactory.Instance.StartUp();

                CommonUtils.DACNavagate(CommonUtils.GetCurrentFrame(), DACPageType.SEARCH, typeof(SearchPage), args.QueryText);
            }
            else
            {
                FirstLaunch(args.QueryText);
            }
        }

        protected override void OnFileActivated(FileActivatedEventArgs args)
        {
            if (args.Files.Count > 0)
            {
                var file = args.Files[0] as Windows.Storage.StorageFile;
                if (file != null)
                {
                    if (args.PreviousExecutionState == ApplicationExecutionState.Running ||
                        args.PreviousExecutionState == ApplicationExecutionState.Suspended)
                    {
                        if (args.PreviousExecutionState == ApplicationExecutionState.Suspended)
                            DACFactory.Instance.StartUp();

                        CommonUtils.GetCurrentFrame().Navigate(typeof(PlayPage), file);
                    }
                    else
                    {
                        FirstLaunch(file);
                    }
                }
            }
        }

        public void SecondaryTileNavgate(string args)
        {
            if (!string.IsNullOrEmpty(args)
                && !args.Contains("@webSite"))
            {
                var frame = CommonUtils.GetCurrentFrame();
                while (frame.CanGoBack)
                    frame.GoBack();

                if (args.Contains(","))
                {
                    var argments = args.Split(',');
                    CommonUtils.DACNavagate(CommonUtils.GetCurrentFrame(), DACPageType.History, 
                        typeof(PlayPage), new PlayInfoHelp(int.Parse(argments[0]), int.Parse(argments[1])));
                }
                else
                {
                    CommonUtils.DACNavagate(CommonUtils.GetCurrentFrame(), DACPageType.FAV,
                        typeof(DetailPage), args);
                }
            }
        }

        private void FirstLaunch(object parameter)
        {
            RegisterShareEvent();
            SettingsPane.GetForCurrentView().CommandsRequested += App_SettingRequested;
            SearchPane.GetForCurrentView().QuerySubmitted += App_SearchRequested;

            var rootFrame = new Frame();
            Window.Current.Content = rootFrame;
            rootFrame.Navigate(typeof(MainPage), parameter);
            Window.Current.Activate();
        }

        private void App_SettingRequested(SettingsPane sender, SettingsPaneCommandsRequestedEventArgs args)
        {
            var polices = new SettingsCommand("polices", "隐私策略", async handler =>
            {
                await Windows.System.Launcher.LaunchUriAsync(Utils.Constants.HttpPolicesUri);
            });

            var about = new SettingsCommand("about", "关于", handler =>
            {
                var settings = new SettingsFlyout();
                settings.ShowFlyout(new About(), 346, true);
            });

            var download = new SettingsCommand("download", "下载路径设置", handler =>
            {
                CommonUtils.DownloadFolderSelect();
            });

            args.Request.ApplicationCommands.Add(download);
            args.Request.ApplicationCommands.Add(polices);
            args.Request.ApplicationCommands.Add(about);
        }

        private void App_SearchRequested(SearchPane sender, SearchPaneQuerySubmittedEventArgs args)
        {
            CommonUtils.GetCurrentFrame().Navigate(typeof(SearchPage), args.QueryText);
        }

        private void RegisterShareEvent()
        {
            DataTransferManager.GetForCurrentView().DataRequested += OnDataRequested;
            DataTransferManager.GetForCurrentView().TargetApplicationChosen += App_TargetApplicationChosen;
        }

        void App_TargetApplicationChosen(DataTransferManager sender, TargetApplicationChosenEventArgs args)
        {
            WAYLocalFactory.Instance.AddShareCount();
        }

        private void OnDataRequested(DataTransferManager sender, DataRequestedEventArgs args)
        {
            ShareUtils.Share(args.Request);
        }
    }
}