#define USINGABB
#include <Stdcjz.h>
BOOL CJZAPI IsDir(const string& lpPath)
{	//是否是文件夹 
	struct _stat buf;
	int res;
	res = _stat(lpPath.c_str(), &buf);
	return (buf.st_mode & _S_IFDIR);
}
BOOL CJZAPI IsFile(const string& lpPath)
{	//是否是文件 
	struct _stat buf;
	int res;
	res = _stat(lpPath.c_str(), &buf);
	return (buf.st_mode & _S_IFREG);
}
string CJZAPI strtail(string s, int cnt = 1) {
	//012 len=3
	//abc   s.substr(2,1)
	if (cnt > s.size())
		return s;
	return s.substr(s.size() - cnt, cnt);
}
string CJZAPI strhead(string s, int cnt = 1) {
	if (cnt > s.size())
		return s;
	return s.substr(0, cnt);
}
string CJZAPI strxtail(string s, int cnt = 1) {
	if (cnt > s.size())
		return "";
	return s.substr(0, s.size() - cnt);
}
string CJZAPI strxhead(string s, int cnt = 1) {
	if (cnt > s.size())
		return "";
	return s.substr(cnt, s.size() - cnt);
}
vector<string> CJZAPI GetDirFiles(string dir, string filter = "*.*")
{
	if (dir.empty() || (filter != "" && !ExistFile(dir)))
	{
		return vector<string>();
	}
	_finddata_t fileDir;
	long lfDir;
	vector<string> files{};
	string dirp = dir + filter;	//它是查找式的

	if ((lfDir = _findfirst(dirp.c_str(), &fileDir)) == -1l)
	{
		return vector<string>();
	}
	else {
		do {	//遍历目录
			if (!strequ(fileDir.name, ".") && !strequ(fileDir.name, ".."))	//排除这两个狗东西
				files.push_back(string(fileDir.name));
		} while (_findnext(lfDir, &fileDir) == 0);
	}
	_findclose(lfDir);
	return files;
}
void GetDirFilesR_Proc(vector<string>* result, const string& odir /*backslashed*/, const string& childDir, const string& filter)
{
	vector<string> matchedFiles = GetDirFiles(odir+childDir, filter);
	for (auto& f : matchedFiles)
		if (IsFile(odir+childDir+f))
		{
			cout<<childDir+f<<endl;
			result->push_back(childDir + f);
			cout<<"---Result size="<<result->size()<<endl;
		}
	matchedFiles.clear();
	vector<string> all = GetDirFiles(odir + childDir, "*");
	for (auto& ele : all)
		if (IsDir(odir+childDir+ele))
		{
			cout<<"   * into: "<<childDir+ele<<endl;
			GetDirFilesR_Proc(result, odir, childDir + ele + "\\", filter);
		}
			
}
vector<string> CJZAPI GetDirFilesR(const string& dir /*backslashed*/, const string& filter = "*.*")
{
	vector<string> result;
	cout<<"\nDIR="<<dir;
	cout<<"\nFILTER="<<filter;
	cout<<"\n\n==============================================";
	GetDirFilesR_Proc(&result, dir, "", filter);
	return result;
}
int main()
{
	vector<string> files = GetDirFilesR(GetOwnDir(true),"*.*");
	for(auto& s : files)
	{
		cout<<s;
		if((s.size() < 4 || strtail(s,4) != ".mp3")
		&& s.find('.')==string::npos)
		{
			cout<<" no ext";
			rename(s.c_str(), (s+".mp3").c_str());
		}
		if((s.size() > 8 && strhead(s,4) == "snd_") && strtail(s,4) == ".mp3")
		{
			cout<<" has snd_";
			rename(s.c_str(),strxhead(s,4).c_str());
		}
		if(s.size() > 9 && strtail(s,9) == " (V0).mp3")
		{
			cout<<" has v0";
			rename(s.c_str(),(strxtail(s,9)+".mp3").c_str());
		}else if(s.size() > 8 && strtail(s,8) == " (1).mp3")
		{
			cout<<" has 1";
			rename(s.c_str(),(strxtail(s,8)+".mp3").c_str());
		}else if(s.size() > 8 && strtail(s,8) == " (2).mp3")
		{
			cout<<" has 2";
			rename(s.c_str(),(strxtail(s,8)+".mp3").c_str());
		}else if(s.size() > 8 && strtail(s,8) == " (3).mp3")
		{
			cout<<" has 3";
			rename(s.c_str(),(strxtail(s,8)+".mp3").c_str());
		}
		cout<<endl;
	}
	return 0;
}
