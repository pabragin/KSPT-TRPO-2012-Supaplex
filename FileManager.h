#include <stdio.h>
#include <vector>
#include <string>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include <algorithm>
#include "stdafx.h"

class FileManager
{
	private:
		vector<string> Files;
		vector<string> Folders;
	public:
		FileManager();
		~FileManager();
		vector<string> GetFiles();
		vector<string> GetFolders();
		void ReadFolder(string path);
};
