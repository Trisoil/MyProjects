using System;
using System.Net;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Documents;
using System.Windows.Ink;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Animation;
using System.Windows.Shapes;
using System.IO.IsolatedStorage;
using System.Collections.ObjectModel;
using System.IO;
using System.Runtime.Serialization.Json;
using PPTVData.Entity;
namespace PPTVData.Factory
{
    public class ExceptionInfoFactory
    {
        private static JsonDataStore<ExceptionCollectionInfo> _jsonDataStore = new JsonDataStore<ExceptionCollectionInfo>("exception.db");
        public ObservableCollection<ExceptionCollectionInfo> ExceptionCollectionInfo
        {
            get { return _jsonDataStore.Items; }
        }
        public void Save(ExceptionCollectionInfo info)
        {
            _jsonDataStore.Items.Add(info);
            _jsonDataStore.Commit();
        }
    }

    public class JsonDataStore<T>
    {
        public JsonDataStore(string fileName)
        {
            _fileName = fileName;
            Items = new ObservableCollection<T>();
            LoadItemsFromIsolatedStorage();
        }

        private string _fileName;

        public ObservableCollection<T> Items
        {
            get;
            private set;
        }

        public void Rollback()
        {
            LoadItemsFromIsolatedStorage();
        }

        public void Commit()
        {
            using (var fileStore = IsolatedStorageFile.GetUserStoreForApplication())
            using (var fileStream = fileStore.OpenFile(_fileName, FileMode.Create))
            {
                var jsonSerializer = new DataContractJsonSerializer(typeof(ObservableCollection<T>));
                jsonSerializer.WriteObject(fileStream, Items);
            }
        }

        public void Clear()
        {
            using (var fileStore = IsolatedStorageFile.GetUserStoreForApplication())
            {
                if (fileStore.FileExists(_fileName))
                {
                    fileStore.DeleteFile(_fileName);
                }
            }
        }

        private void LoadItemsFromIsolatedStorage()
        {
            using (var fileStore = IsolatedStorageFile.GetUserStoreForApplication())
            {
                if (fileStore.FileExists(_fileName))
                {
                    using (var fileStream = fileStore.OpenFile(_fileName, FileMode.Open))
                    {
                        Items.Clear();
                        var jsonSerializer = new DataContractJsonSerializer(typeof(ObservableCollection<T>));
                        ((ObservableCollection<T>)jsonSerializer.ReadObject(fileStream)).ForEach(Items.Add);
                    }
                }
            }
        }
    }
}
