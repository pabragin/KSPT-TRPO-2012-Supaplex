#include "FileManager.h"
#define ISFILE 0x8
#define ISFOLDER 0x4

FileManager::FileManager()
{
}
FileManager::~FileManager()
{
}

void FileManager::ReadFolder(string path)
{
    int maxsize=path.size();
    char cPath[maxsize];
    for (int i=0;i<=maxsize;i++)
    {
	cPath[i]=path[i];
    }
     char *dirname;
     DIR *dir;
     struct dirent *entry;
     struct stat statinfo;
	 dirname = cPath;
	 Folders.clear();
	 Files.clear();
     if ((dir = opendir(dirname)) == NULL) 
     {
	  return;
     }
     chdir(dirname);
     while ((entry = readdir(dir)) != NULL) {
	  stat(entry->d_name, &statinfo);
	  if (S_ISDIR(statinfo.st_mode))
		{
				Folders.push_back(entry->d_name);
		}
	  else if (S_ISREG(statinfo.st_mode)) 
	  {
	       Files.push_back(entry->d_name);
	  }
     }
     sort(Files.begin(), Files.end());
     sort(Folders.begin(), Folders.end());
     closedir(dir);
}

vector<string> FileManager::GetFiles()
{
	return this->Files;
}

vector<string> FileManager::GetFolders()
{
	return this->Folders;
}
