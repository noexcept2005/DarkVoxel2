/***********************************
 *           Dark Voxel 2          *
 *           精彩重制版            *
 *        Author: Wormwaker        *
 * Copyright (C)2023-Wormwake Games*
 *     All  Rights  Reserved.      *
 *     Do  Not  Distribute!        *
 * 任何人未经允许不得查看/修改/发布*
 ***********************************
 */
//32位程序 (x86)
//#define CAREFUL_TRACK					//调试跟踪 
#define ITEM_REAL_SHAPE
#define NO_NVMAP_BLURRING
#define CURRENT_VERSION "v2.0.1.3"
//Extra Compiling Options
//-lgdi32 -luser32 -mwindows -lwininet -lws2_32 -lgraphics64 -luuid -lmsimg32 -limm32 -lole32 -loleaut32 -lgdiplus -lwinmm
#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#pragma warning(disable:4018)
#pragma warning(disable:4102)
#pragma warning(disable:4244)	//lose precision
#include <windows.h>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <io.h>
#include <tchar.h>
#include <string>
#include <fstream> 
#include <sstream>
#include <stdexcept>
#include <cmath>
#include <algorithm>
#include <random>
#include <chrono>
#include <vector>
#include <array>
#include <queue>
#include <stack>
#include <list>
#include <map>
#include "graphics.h"	//Use EGE 
#include "ege\\sys_edit.h"
#pragma comment(lib,"graphics17.lib")
using namespace std;
#ifdef small
#undef small
#endif
#define INVALID_UUID 0
#define XCPT_L try {
#define XCPT_R }catch(exception& e)\
	{\
		DebugLog(string(__func__) + "EXCEPTION: " + (string) e.what(), DLT_ERROR);\
		MessageBox(NULL,("程序抛出异常！内容如下：\n"+(string) e.what()).c_str(), (string("DarkVoxel2 ") + __func__ + string(" ERROR")).c_str(),MB_ICONERROR|MB_OK);\
	}
#define repeat(a)	for(short hard_to_same_var = 0; hard_to_same_var < a; ++hard_to_same_var)
#define fequ(f1,f2)	(abs(f1-f2) < 0.001f)
#define KEY_CD 100
HWND hwnd=nullptr;
int uix=0,uiy=0;

string DV_DIR;			//DarkVoxel2目录路径
string LOCAL_SAVE_DIR;	//存档目录路径
string LOCAL_DATA_DIR;	//数据目录路径

struct LCOORD {
	long X;
	long Y;
	LCOORD():X{0L},Y{0L}	{}
	LCOORD(long _x, long _y) : X(_x) , Y(_y)	{}
};
struct DCOORD {
	double X;
	double Y;
	DCOORD(double _x, double _y) : X(_x), Y(_y) {}
};

#define CJZAPI __stdcall
//字符串处理函数
template <typename _T>
string CJZAPI ToString(_T value) {
	stringstream ss;
	ss<<value;
	return ss.str();
}
template <typename _T>	//模板定制，有些人叫模板特例
string CJZAPI ToString(const vector<_T> _list) {
	//列表转字符串
	string res="[";
	for(short i = 0; i < _list.size(); ++i) {
		res += ToString(_list.at(i));
		if(i != _list.size() - 1)
			res += ",";
	}
	res += "]";
	return res;
}
template <typename _T1, typename _T2>
_T2 CJZAPI Convert(_T1 src) {
	stringstream ss;
	_T2 ret;
	ss<<src;
	ss>>ret;
	return ret;
}
template <typename _T>
bool Contain(const vector<_T>& container, _T element)
{
	for (const auto& ele : container)
		if (ele == element)
			return true;
	return false;
}
#define LOG_NAME "prevPlaying.log"
#define DLT_INFO 0
#define DLT_WARN 1
#define DLT_WARNING 1
#define DLT_ERROR 2
#define DLT_TRACK 3
template <typename _AnyType>
void DebugLog(_AnyType sth, UINT uType = DLT_INFO) {
	//日志输出
	FILE *fp=fopen(LOG_NAME,"a+");
	char prefix[10];
	if(uType == DLT_INFO)
		strcpy(prefix,"[INFO]");
	else if(uType == DLT_WARN)
		strcpy(prefix,"<!WARN>");
	else if(uType == DLT_TRACK)
		strcpy(prefix,"跟踪：");
	else {
		strcpy(prefix,"(xERROR)");
	}
	//fprintf(fp,"\n%s%s",bTime?(("["+string(ToTimeStr(time(0)))+"] ").c_str()):"",text.c_str());
	fprintf(fp, "\n%s %s", prefix, ToString(sth).c_str());
	fclose(fp);
}
template <>
void DebugLog(string text, UINT uType) {
	FILE *fp=fopen(LOG_NAME,"a+");
	char prefix[10];
	if(uType == DLT_INFO)
		strcpy(prefix,"[INFO]");
	else if(uType == DLT_WARN)
		strcpy(prefix,"<!WARN>");
	else if(uType == DLT_TRACK)
		strcpy(prefix,"跟踪：");
	else {
		strcpy(prefix,"(xERROR)");
	}
	//fprintf(fp,"\n%s%s",bTime?(("["+string(ToTimeStr(time(0)))+"] ").c_str()):"",text.c_str());
	fprintf(fp, "\n%s %s", prefix, text.c_str());
	fclose(fp);
}
#ifdef CAREFUL_TRACK
#define TrackLog(s)	DebugLog(" "+::ToString(__func__)+":"+::ToString(s),DLT_TRACK)
#else
#define TrackLog(...)	(__VA_ARGS__)
#endif
inline void WarnLog(string sText, string moduleName="") {
	if(moduleName != "")
		moduleName += ": ";
	DebugLog(moduleName+sText, DLT_WARN);
}
inline void ErrorLog(string sText, string moduleName="") {
	if(moduleName != "")
		moduleName += ": ";
	DebugLog(moduleName+sText, DLT_ERROR);
}

//颜色
//Reference from https://tool.oschina.net/commons?type=3

//橙黄系列
#define IVORY EGERGB(255,255,240)	//极淡黄 
#define CORNSILK EGERGB(255,248,220)	//淡黄 
#define GOLD EGERGB(255,215,0)
#define ORANGE EGERGB(255,165,0)

//绿色系列
#define DARKGREEN EGERGB(0,100,0)

//蓝系列
#define POWDERBLUE EGERGB(176,224,230)
#define AQUA EGERGB(0,0xFC,0xFC)
#define ROYALBLUE EGERGB(65,105,225)
#define STEELBLUE EGERGB(70,130,180)
#define CADETBLUE EGERGB(95,158,160)
#define LIGHTBLUE1 EGERGB(191,239,255)
#define LIGHTCYAN1 EGERGB(224,255,255)
#define TURQUOISE EGERGB(64,224,208)	//青色 

//粉紫系列
#define PINK EGERGB(255,105,180)
#define LIGHTPINK EGERGB(255,182,193)
#define MAROON EGERGB(176,48,96)
#define VIOLET EGERGB(238,130,238)
#define PLUM EGERGB(221,160,221)
#define PURPLE EGERGB(160,32,240)

//黑白系列
#define GRAY_11 EGERGB(28,28,28)
#define DIMGRAY EGERGB(105,105,105)
#define SLATEGRAY EGERGB(112,128,144)	//略带蓝色

COLORREF CJZAPI Color16ToRGB(UINT u16Clr) {
	switch(u16Clr) {
		case 0: {
			return RGB(0,0,0);		//黑
			break;
		}
		case 1: {
			return RGB(0,0,128);	//深蓝
			break;
		}
		case 2: {
			return RGB(0,128,0);	//草绿
			break;
		}
		case 3: {
			return RGB(0,128,128);	//青色
			break;
		}
		case 4: {
			return RGB(128,0,0);	//Dark Red
			break;
		}
		case 5: {
			return RGB(128,0,128);	//Dark Magenta
			break;
		}
		case 6: {
			return RGB(128,128,0);	//Brown
			break;
		}
		case 7: {
			return RGB(192,192,192);	//浅灰
			break;
		}
		case 8: {
			return RGB(128,128,128);	//深灰
			break;
		}
		case 9: {
			return RGB(0,0,255);	//海蓝
			break;
		}
		case 10: {
			return RGB(0,255,0);	//淡绿
			break;
		}
		case 11: {
			return RGB(0,255,255);	//淡蓝
			break;
		}
		case 12: {
			return RGB(255,0,0);	//血红
			break;
		}
		case 13: {
			return RGB(255,0,255);	//Magenta 紫色
			break;
		}
		case 14: {
			return RGB(255,255,0);	//淡黄
			break;
		}
		case 15: {
			return RGB(255,255,255);	//白
			break;
		}
		default: {
			return RGB(0,0,0);
			break;
		}
	}
}

COLORREF CJZAPI StepColor0(COLORREF startColor, COLORREF endColor, double rate) {
	if(rate == 0.0)	return startColor;
	if(rate == 1.0) return endColor;
	//颜色的渐变
	int r = (GetRValue(endColor) - GetRValue(startColor));
	int g = (GetGValue(endColor) - GetGValue(startColor));
	int b = (GetBValue(endColor) - GetBValue(startColor));

	int nSteps = max(abs(r), max(abs(g), abs(b)));
	if (nSteps < 1) nSteps = 1;

	// Calculate the step size for each color
	float rStep = r / (float)nSteps;
	float gStep = g / (float)nSteps;
	float bStep = b / (float)nSteps;

	// Reset the colors to the starting position
	float fr = GetRValue(startColor);
	float fg = GetGValue(startColor);
	float fb = GetBValue(startColor);

	COLORREF color;
	for (int i = 0; i < int(nSteps * rate); i++) {
		fr += rStep;
		fg += gStep;
		fb += bStep;
		color = RGB((int)(fr + 0.5), (int)(fg + 0.5), (int)(fb + 0.5));
		//color 即为重建颜色
	}
	return color;
}//from https://bbs.csdn.net/topics/240006897 , owner: zgl7903
inline COLORREF CJZAPI WinColor(color_t clr) {
	//EGERGB -> RGB
	return RGB(EGEGET_R(clr),EGEGET_G(clr),EGEGET_B(clr));
}
inline color_t CJZAPI EgeColor(COLORREF clr) {
	//RGB -> EGERGB
	return EGERGB(GetRValue(clr),GetGValue(clr),GetBValue(clr));
}
inline color_t CJZAPI StepColor(color_t startColor, color_t endColor, double rate) {
	return EgeColor(StepColor0(WinColor(startColor),WinColor(endColor),rate));
}
inline color_t CJZAPI InvertedColor(color_t clr) {
	return EGERGB(255-EGEGET_R(clr),255-EGEGET_G(clr),255-EGEGET_B(clr));
}

#define DS_GRAY_GREEN 1
#define DS_DEFAULT DS_GRAY_GREEN
#define DS_RED_GREEN 2
#define DS_DARK_RED_GREEN 3
#define DS_ON_OFF 4
#define DS_KAI_GUAN 5

string CJZAPI GetSwitch(bool isOn ,color_t &color, UINT mode = DS_GRAY_GREEN) {
	//开关字符串
	short pattern_mode=0;
	string s="";
	if(mode == DS_GRAY_GREEN) {
		color = ((isOn?LIGHTGREEN:DARKGRAY));
	} else if(mode == DS_RED_GREEN) {
		color = ((isOn?LIGHTGREEN:LIGHTRED));
	} else if(mode == DS_DARK_RED_GREEN) {
		color = ((isOn ? GREEN:RED));
	} else if(mode == DS_ON_OFF) {
		color = ((isOn?LIGHTGREEN:LIGHTRED));
		pattern_mode=1;
	} else if(mode == DS_KAI_GUAN) {
		color = ((isOn?LIGHTGREEN:LIGHTRED));
		pattern_mode=2;
	}
	if(pattern_mode==0) {
		if(!isOn)	s+="■コ";
		else		s+="ㄈ■";
	} else if(pattern_mode==1) {
		if(isOn)	s+="ON";
		else		s+="OFF";
	} else if(pattern_mode==2) {
		if(isOn)	s+="开";
		else		s+="关";
	}
	return s;
}
inline long CJZAPI HexToDec(const string& s)
{
	stringstream ss;
	ss << s;
	long res = 0;
	ss >> hex >> res;
	return res;
}
template <typename _T>
inline _T CJZAPI pow2(const _T& x)
{
	return x * x;
}
inline double CJZAPI Distance(double x1, double y1, double x2, double y2) {
	return sqrt((x1-x2)*(x1-x2)+(y1-y2)*(y1-y2));
}
inline double CJZAPI Distance(long x1, long y1, long x2, long y2) {
	return sqrt((x1-x2)*(x1-x2)+(y1-y2)*(y1-y2));
}
template <typename _T>
inline _T CJZAPI ClampA(_T& val, _T min=0, _T max=2147483647) {	//限定范围
	if(val < min) val = min;
	else if(val > max) val = max;
	return val;
}
template <typename _T>
inline _T CJZAPI Clamp(_T val, _T min=0, _T max=2147483647) {	//限定范围
	if(val < min) val = min;
	else if(val > max) val = max;
	return val;
}
template <typename _T>
inline double CJZAPI Lerp(_T startValue, _T endValue, double ratio)
{
	return startValue + (endValue - startValue) * ratio;
}
inline double CJZAPI EaseInOutSine(double _x)
{	//retval,_x ∈ [0,1]
	return -(cos(PI * _x) - 1) / 2;
}
inline double CJZAPI EaseInOutBack(double _x)
{
	const double c1 = 1.70158;
	const double c2 = c1 * 1.525;
	return _x < 0.5
		? (pow(2 * _x, 2) * ((c2 + 1) * 2 * _x - c2)) / 2
		: (pow(2 * _x - 2, 2) * ((c2 + 1) * (_x * 2 - 2) + c2) + 2) / 2;
}
inline double CJZAPI EaseOutCubic(double _x)
{
	return 1 - pow(1 - _x, 3);
}
inline double CJZAPI EaseInOutElastic(double _x)
{
	const double c5 = (2 * PI) / 4.5;
	return fequ(_x, 0.0)
		? 0.0
		: fequ(_x, 1.0)
		? 1.0
		: _x < 0.5
		? -(pow(2, 20 * _x - 10) * sin((20 * _x - 11.125) * c5)) / 2
		: (pow(2, -20 * _x + 10) * sin((20 * _x - 11.125) * c5)) / 2 + 1;
}
inline double CJZAPI EaseOutBounce(double _x)
{
	const double n1 = 7.5625;
	const double d1 = 2.75;

	if (_x < 1 / d1) {
		return n1 * _x * _x;
	}
	else if (_x < 2 / d1) {
		return n1 * (_x -= 1.5 / d1) * _x + 0.75;
	}
	else if (_x < 2.5 / d1) {
		return n1 * (_x -= 2.25 / d1) * _x + 0.9375;
	}
	else {
		return n1 * (_x -= 2.625 / d1) * _x + 0.984375;
	}
}
inline double CJZAPI EaseInOutBounce(double _x)
{
	return _x < 0.5
		? (1 - EaseOutBounce(1 - 2 * _x)) / 2
		: (1 + EaseOutBounce(2 * _x - 1)) / 2;
}
inline double CJZAPI EaseInOutExpo(double _x)
{
	return fequ(_x, 0.0)
		? 0.0
		: fequ(_x, 1.0)
		? 1.0
		: _x < 0.5 ? pow(2, 20 * _x - 10) / 2
		: (2 - pow(2, -20 * _x + 10)) / 2;
}
int CJZAPI RandomRange(int Min=0,int Max=32767,bool rchMin=true,bool rchMax=true) {
	//随机数值区间
	int a;
	a = random(Max);

	if(rchMin && rchMax)	//[a,b]
		return (a%(Max - Min + 1)) + Min;
	else if(rchMin && !rchMax)		//[a,b)
		return (a%(Max - Min)) + Min;
	else if(!rchMin && rchMax)		//(a,b]
		return (a%(Max - Min)) + Min + 1;
	else							//(a,b)
		return (a%(Max - Min - 1)) + Min + 1;
}
double CJZAPI RandomRangeDouble(double _min, double _max,	//min,max
	bool rchMin = true, bool rchMax = true,	//开/闭 
	UINT uPrec = 2	//精度（位数） 
)
{	//随机小数区间 
	double p_small = pow(10.0f, -int(uPrec));
	int p_big = (int)pow(10, uPrec);
	int l = int(p_big*_min);
	int r = int(p_big*_max);
	int res;
	res = RandomRange(l, r, rchMin, rchMax);
	return (p_small*(double)res);
}
template<typename _T>
inline _T CJZAPI Choice(initializer_list<_T> choices) {
	vector<_T> vec(choices);
	return vec[RandomRange(0, vec.size())];
}
template<typename _T>
inline _T CJZAPI Choice(const vector<_T>& choices_vector) {
	return choices_vector[RandomRange(0, choices_vector.size())];
}
template<typename _T>
inline _T& CJZAPI ChoiceRef(vector<_T>& choices_vector) {
	return choices_vector[RandomRange(0, choices_vector.size())];
}
template<typename _T>
inline bool CJZAPI Percent(_T prob) {
	return (RandomRange(0,100,true,false)<prob);
}
template<typename _T>
inline bool CJZAPI Permille(_T prob) {	//千分数
	return (RandomRange(0, 1000, true, false) < prob);
}

inline BOOL CJZAPI ExistFile(string strFile) {
	//文件或文件夹都可以
	return !_access(strFile.c_str(),S_OK);//S_OK表示只检查是否存在
}
VOID CJZAPI CreateFile(LPCSTR wholePath,LPCSTR text) {
	FILE *fp=nullptr;
	fp=fopen(wholePath,"w");
	fputs(text,fp);
	fclose(fp);
	return;
}
inline bool CJZAPI strequ(char *str, const char *obj)
{	//比较是否一样 
	return (strcmp((const char *)str, obj) == 0 ? true : false);
}
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
vector<string> CJZAPI GetDirFiles(const string& dir, const string& filter = "*.*")
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
	vector<string> matchedFiles = GetDirFiles(odir + childDir, filter);
	for (auto& f : matchedFiles)
		if (IsFile(odir + childDir + f))
		{
			result->push_back(childDir + f);
		}
	matchedFiles.clear();
	vector<string> all = GetDirFiles(odir + childDir, "*");
	for (auto& ele : all)
		if (IsDir(odir + childDir + ele))
		{
			GetDirFilesR_Proc(result, odir, childDir + ele + "\\", filter);
		}

}
vector<string> CJZAPI GetDirFilesR(const string& dir /*backslashed*/, const string& filter = "*.*")
{
	vector<string> result;
	GetDirFilesR_Proc(&result, dir, "", filter);
	return result;
}
void CJZAPI GetMousePos(int& mx, int& my)
{
	POINT pt;
	GetCursorPos(&pt);
	ScreenToClient(hwnd,&pt);
	mx = pt.x;
	my = pt.y;
}
inline bool CJZAPI strcaseequ(char *str, const char *obj) {
	//无视大小写的比较
	return (_stricmp((const char *)str, obj) == 0 ? true : false);
}
inline bool CJZAPI sequ(string s, string obj) {
	return strcaseequ((char*)s.c_str(), obj.c_str());
}
const char * CJZAPI strcat_cc(const char *s1,const char *s2) {
	stringstream ss;
	string s;
	ss<<s1<<s2;
	s=ss.str();
	return s.c_str();
}
//开启线程简写
HANDLE CJZAPI CreateThread(LPTHREAD_START_ROUTINE func_name, LPVOID args=nullptr) {
	// ###警告！！函数格式必须为 DWORD FUNC(LPVOID XXX) ###
	return ::CreateThread(NULL,0,func_name, args, 0,NULL);
}
inline RECT CJZAPI GetSystemWorkAreaRect(void) { //获取工作区矩形
	RECT rt;
	SystemParametersInfo(SPI_GETWORKAREA,0,&rt,0);    // 获得工作区大小
	return rt;
}
inline LONG CJZAPI GetSystemWorkAreaHeight(void) { //高
	RECT rt=GetSystemWorkAreaRect();
	return (rt.bottom - rt.top);
}
inline LONG CJZAPI GetSystemWorkAreaWidth(void) {	//宽
	RECT rt=GetSystemWorkAreaRect();
	return (rt.right - rt.left);
}
inline VOID CJZAPI InfoTip(HWND hwnd=nullptr,string strText="Information",string strTitle="InfoTip") {
	MessageBox(hwnd,strText.c_str(),strTitle.c_str(),MB_ICONINFORMATION|MB_OK|MB_SYSTEMMODAL);
}
inline VOID CJZAPI ExclaTip(HWND hwnd=nullptr,string strText="Exclamation",string strTitle="ExclaTip") {
	MessageBox(hwnd,strText.c_str(),strTitle.c_str(),MB_ICONEXCLAMATION|MB_OK|MB_SYSTEMMODAL);
}
inline VOID CJZAPI ErrorTip(HWND hwnd=nullptr,string strText="Error",string strTitle="ErrorTip") {
	MessageBox(hwnd,strText.c_str(),strTitle.c_str(),MB_ICONERROR|MB_OK|MB_SYSTEMMODAL);
}
inline VOID CJZAPI MsgSndTip(VOID) {
	MessageBeep(MB_ICONINFORMATION);
}
inline bool CJZAPI InRect(int x,int y,int x_left,int y_top,int x_right,int y_bottom) {
	if(x >= x_left && x <= x_right && y >= y_top && y <= y_bottom)
		return true;
	return false;
}
inline bool CJZAPI InRect(int x,int y,RECT& rt) {
	if(x >= rt.left && x <= rt.right && y >= rt.top && y <= rt.bottom)
		return true;
	return false;
}
char * CJZAPI strrpc(char *str,const char *oldstr,const char *newstr) {
	//字符串替换
	char bstr[65535];//转换缓冲区
	memset(bstr,0,sizeof(bstr));
	for(size_t i = 0; i < strlen(str); i++) {
		if(!strncmp(str+i,oldstr,strlen(oldstr))) {
			//查找目标字符串
			strcat(bstr,newstr);
			i += strlen(oldstr) - 1;
		} else {
			strncat(bstr,str + i,1);//保存一字节进缓冲区
		}
	}
	strcpy(str,bstr);
	return str;
}
//from https://blog.csdn.net/qq_41673920/article/details/81390972
string CJZAPI strtail(const string& s, int cnt = 1) {
	//012 len=3
	//abc   s.substr(2,1)
	if (cnt > s.size())
		return s;
	return s.substr(s.size() - cnt, cnt);
}
string CJZAPI strhead(const string& s, int cnt = 1) {
	if (cnt > s.size())
		return s;
	return s.substr(0, cnt);
}
string CJZAPI strxtail(const string& s, int cnt = 1) {
	if (cnt > s.size())
		return "";
	return s.substr(0, s.size() - cnt);
}
string CJZAPI strxhead(const string& s, int cnt = 1) {
	if (cnt > s.size())
		return "";
	return s.substr(cnt, s.size() - cnt);
}
inline string CJZAPI unquote(const string& src) {
	return strxtail(strxhead(src));
}
inline void CJZAPI WarnLogTip(HWND hwnd, const string& strText,
                              const string& moduleName=""
                             ) {
	string db=moduleName+" WARNING: "+strText;
	db = strrpc((char*)db.c_str(),"\n"," | ");	//删除内部换行
	DebugLog(db,DLT_WARN);
	ErrorTip(hwnd,strText,"DarkVoxel2 "+moduleName+" WARNING");
}
inline void CJZAPI ErrorLogTip(HWND hwnd, const string& strText,
                              const string& moduleName=""
                              ) {
	string db=moduleName+" ERROR: "+strText;
	db = strrpc((char*)db.c_str(),"\n"," | ");	//删除内部换行
	DebugLog(db,DLT_ERROR);
	ErrorTip(hwnd,strText,"DarkVoxel2 "+moduleName+" ERROR");
}
int g_lineNum;	//行数 , 从零开始

void CJZAPI initprint(void) {
	g_lineNum=0;
}
void CJZAPI cls(bool bZeroLineNum=true) {
	//清屏
	cleardevice();
	if(bZeroLineNum)
		g_lineNum=0;
}

#define CUR_LINE_NUM (g_lineNum)	//当前行 
#define NEXT_LINE_NUM (++g_lineNum)	//下一行 
inline short KnowItsLen(u_char c) {
	short ret = 0;
	if(c > 0x7f)	ret = 2;
	else			ret = 1;
	/*char buf[10];
	sprintf(buf,"%d  %c  %s",(unsigned int)c,c,str(ret).c_str());
	DebugLog(buf);*/
	return ret;
}
RECT CJZAPI midprint(const string& strText,int lineNum=NEXT_LINE_NUM,int _left=0, int _right=uix, DWORD dwInternalDelayMs=0) {
	//居中显示句子
	int sz=textheight('1');	//font height
	int stotx = textwidth(strText.c_str());
	int x=_left+(_right-_left)/2-stotx/2;
	int y=sz*lineNum;
	short add = 0;
	for(short i = 0; i < strText.length() - strText.length()%2;) {
		string tmp = strText.substr(i,KnowItsLen(strText.at(i)));
		xyprintf(x+add,y,"%s",tmp.c_str());
		if(tmp.size() == 1) {
			add += sz/2;
			i += 1;
		} else {
			add += sz;
			i += 2;
		}
		if(dwInternalDelayMs > 0) {
			delay_fps(60);
			delay(dwInternalDelayMs-1000.0/60.0);
		}
	}
	RECT rt;
	rt.left = x;
	rt.right = x+stotx;
	rt.top = y;
	rt.bottom = y+sz;
	return rt;	//返回矩形
}
RECT CJZAPI midprinty(const string& strText,int y,int _left=0, int _right=uix, DWORD dwInternalDelayMs=0) {
	//自定义 y
	int sz=textheight('1');	//font height
	int stotx = textwidth(strText.c_str());
	int x=_left+(_right-_left)/2-stotx/2;
	short add = 0;
	for(short i = 0; i < strText.length() - strText.length()%2;) {
		string tmp = strText.substr(i,KnowItsLen(strText.at(i)));
		xyprintf(x+add,y,"%s",tmp.c_str());
		if(tmp.size() == 1) {
			add += sz/2;
			i += 1;
		} else {
			add += sz;
			i += 2;
		}
		if(dwInternalDelayMs > 0) {
			delay_fps(60);
			delay(dwInternalDelayMs-1000.0/60.0);
		}
	}
	RECT rt;
	rt.left = x;
	rt.right = x+stotx;
	rt.top = y;
	rt.bottom = y+sz;
	return rt;	//返回矩形
}

vector<string> CJZAPI CutLine(const string& line,char sep=' ') {
	vector<string> ret {};
	if(line.empty()) return ret;
	int p1=0,p2=0;
	for(int i=0; i<line.size(); i++) {
		if(line.at(i) == sep
		        || i==line.size()-1) {
			p2=i;
			string s=line.substr(p1,p2-p1+(i==line.size()-1?1:0));
			ret.push_back(s);
			p1=i+1;
		}
	}
	return ret;
}
short CJZAPI RangeChoice(const string& s) {
	if(s.empty())	return 0;
	vector<string> items=CutLine(s,'~');
	if(items.size() > 2)	return 0;
	if(items.size() == 1)
		return Convert<string,short>(s);
	return RandomRange(Convert<string,short>(items[0]),Convert<string,short>(items[1]));
}
vector<string> CJZAPI ReadFileLines(string filePath) {
	//读取文件每一行
	vector<string> ret {} ;
	const int lineMaxCharCnt = 32767;
	fstream fin(filePath,ios::in);
	if(fin.fail())
		return ret;
	while(1) {
		CHAR s[lineMaxCharCnt];

		if(fin.eof())
			break;

		fin.getline(s,lineMaxCharCnt);
		//MSGPRINT(s);
//        cout<<s<<endl;
		ret.push_back(ToString(s));
	}
	fin.close();
	return ret;
}

vector<string> CJZAPI CSVCutLine(string line,char sep=',') {
	//数据分开，单独取出
	//CSV 逗号分隔值文件 , 处理略有不同
	vector<string> data {} ;
	int p1=0,p2=0;

	for(int i=0; i<line.size(); i++) {
		if(line.at(i) == sep
		        || /*line.at(i) == '\n' NO USE*/
		        i==line.size()-1) {	//别漏了最后一个数据
			p2=i;
			string v = line.substr(p1,p2-p1
			                       + ((i==line.size()-1
			                           &&	line.at(i)!=sep)	//防止最后多个逗号
			                          ?1:0) );	//别漏了最后一个字符
			if(v.empty())
				break;

//            MSGPRINT(v);
			data.push_back(v);	//<!>警告：有可能会放空的str
			p1=i+1;
		}
	}
	return data;
}

string CJZAPI ResplitLine(string line,char sep=',',char newSep=' ') {
	//重新分隔 并合并成一句
	vector<string> data=CSVCutLine(line,sep);
	string ret;
	for(int i=0; i<data.size(); i++) {
		ret += data.at(i);
//		MSGPRINT(ret);

		if(i != data.size()-1)
			ret += newSep;
	}
	return ret;
}
string CJZAPI CSVResplitLine(string line,char sep=',',char newSep=' ') {
	return ResplitLine(line,sep,newSep);
}

void CJZAPI _resize(PIMAGE pDstImg, int w, int h) {
	//重写一遍
	PIMAGE save = gettarget();
	settarget(pDstImg);
	int width, height;
	width = getwidth();
	height = getheight();
	settarget(save);
	PIMAGE backproc = newimage(w, h);
	//Resize
	putimage(backproc, 0, 0, w, h, pDstImg, 0, 0, width, height);
	//putimage_withalpha(backproc, pDstImg, 0, 0, 0, 0, w, h);
	getimage(pDstImg, backproc, 0, 0, w, h);
	delimage(backproc);
}
#define KEY_DOWN(VK_NONAME) ((GetAsyncKeyState(VK_NONAME)& 0x8000 ? 1:0))
#define KEY_DOWN_FOREMOST(hWnd,vk) (KEY_DOWN(vk) && GetForegroundWindow()==hWnd)	//最前面 
#define KEY_DOWN_FOCUSED(hWnd,vk) KEY_DOWN_FOREMOST(hWnd,vk)	//带焦点 
#define K(sth) KEY_DOWN_FOCUSED(hwnd,sth)

struct ChnItem {
	string chn;
	color_t clr;
	PIMAGE img;
};
vector<ChnItem> chn_items;
struct ChnItemUnicode {
	long unicode_id;
	color_t clr;
	PIMAGE img;
};
vector<ChnItemUnicode> chn_items_unicode;

#define CHN_MIN_X 12
#define CHN_UNICODE_MIN_X 16
#define CHN_FONTNAME "宋体"
#define CHN_UNICODE_FONTNAME "MS Gothic"
#define CHNITEM_MAX 400
#define CHNITEM_UNICODE_MAX 100
PIMAGE AttainChnImage(const string& chn, color_t clr) {
	XCPT_L
	for(short i = 0; i < chn_items.size(); ++i)
		if(chn_items.at(i).chn == chn && chn_items.at(i).clr == clr)
			return chn_items.at(i).img;
	//else create one
	PIMAGE img = newimage(CHN_MIN_X,CHN_MIN_X);
	setfont(CHN_MIN_X,0,CHN_FONTNAME,img);
	setcolor(clr, img);
	outtextxy(0,0,chn.c_str(),img);
	/*PIMAGE img2 = newimage(CHN_MIN_X,CHN_MIN_X);
	putimage_alphatransparent(img2,img,0,0,BLACK,127);
	delimage(img);*/
	ChnItem item;
	item.chn = chn;
	item.clr = clr;
	item.img = img;
	chn_items.push_back(item);
	if (chn_items.size() > CHNITEM_MAX)
	{
		delimage(chn_items.at(0).img);
		chn_items.erase(begin(chn_items));
	}
	return img;
	XCPT_R
}
PIMAGE AttainChnImage(long unicode_id, color_t clr)
{
	for (short i = 0; i < chn_items_unicode.size(); ++i)
		if (chn_items_unicode.at(i).unicode_id == unicode_id && chn_items_unicode.at(i).clr == clr)
			return chn_items_unicode.at(i).img;
	PIMAGE img = newimage(CHN_UNICODE_MIN_X, CHN_UNICODE_MIN_X);
	setfont(CHN_UNICODE_MIN_X, 0, CHN_UNICODE_FONTNAME, img);
	setcolor(clr, img);
	wchar_t wstr[10]{ 0 };
	wsprintfW(wstr, L"%lc", unicode_id);
	outtextxy(0, 0, wstr, img);
	ChnItemUnicode item;
	item.unicode_id = unicode_id;
	item.clr = clr;
	item.img = img;
	chn_items_unicode.push_back(item);
	if (chn_items_unicode.size() > CHNITEM_UNICODE_MAX)
	{
		delimage(chn_items_unicode.at(0).img);
		chn_items_unicode.erase(begin(chn_items_unicode));
	}
	return img;
}
PIMAGE AttainChnImageInstant(long unicode_id, color_t clr, int realSize/*真实字号*/)
{	//<!> WARNING: 必须记得删除！！否则内存泄漏！！！
	for (short i = 0; i < chn_items_unicode.size(); ++i)
		if (chn_items_unicode.at(i).unicode_id == unicode_id && chn_items_unicode.at(i).clr == clr)
			return chn_items_unicode.at(i).img;
	PIMAGE img = newimage(realSize, realSize);
	setfont(realSize, 0, CHN_FONTNAME, img);
	setcolor(clr, img);
	wchar_t wstr[10]{ 0 };
	wsprintfW(wstr, L"%lc", unicode_id);
	outtextxy(0, 0, wstr, img);
	return img;
}
static BYTE g_px_alpha = 0xFF;
void EGEAPI pxsetalpha(int _alpha = 0xFF)
{	//专门用于pxprint系列
	g_px_alpha = _alpha;
}
#define PXPRINT_SHAKE_PROB (1L)	//3%
RECT pxprint(int x, int y, const string& s, int shake_amp = 0, DWORD dwInternalDelayMs=0, PIMAGE pimg = nullptr) {
	XCPT_L
	color_t clr = getcolor();
	int sz = textheight('%');
	short add = 0;
	for(short i = 0; i < s.length() /*- s.length()%2*/;) {
		string tmp = s.substr(i,KnowItsLen(s.at(i)));
		int x_shake = Percent(PXPRINT_SHAKE_PROB) ? Choice({ -shake_amp, shake_amp }) : 0;
		int y_shake = Percent(PXPRINT_SHAKE_PROB) ? Choice({ -shake_amp, shake_amp }) : 0;
		if(tmp.size() == 1) {
			PIMAGE img = newimage(sz, sz);
			setfont(sz, 0, "Determination Mono", img);
			setcolor(clr, img);
			outtextxy(0, 0, tmp.c_str(), img);
			putimage_alphatransparent(pimg, img, x + add + x_shake, y + y_shake, BLACK, g_px_alpha);
			delimage(img);
			add += sz/2;
			i += 1;
		} else {
			PIMAGE img = AttainChnImage(tmp, clr);
			_resize(img,sz,sz);	//重写后的可用的函数
			//		putimage_withalpha(NULL, img, x + sz * i, y);
			//		putimage_withalpha(NULL, img, 80,80);
//			putimage_alphatransparent(NULL,img, x+add, y, clr, 255);
//			putimage(x+add, y, img);
			putimage_alphatransparent(pimg, img, x+add+x_shake, y+y_shake, BLACK, g_px_alpha);
			add += sz;
			i += 2;
		}
		if(dwInternalDelayMs > 0) {
			delay_fps(60);
			delay(dwInternalDelayMs-1000.0/60.0);
		}
	}
	RECT rt;
	rt.left = x;
	rt.right = x+textwidth(s.c_str());
	rt.top = y;
	rt.bottom = y+sz;
	return rt;	//返回矩形
	XCPT_R
}
RECT pxprint_shadow(int x, int y, const string& s, int shake_amp = 0, PIMAGE pimg = nullptr) {
	auto color = getcolor();
	setcolor(EGERGB(1,1,1));
	pxprint(x + 2, y + 2, s, shake_amp, 0, pimg);
	//pxprint(x - 2, y + 2, s);
	//pxprint(x + 2, y - 2, s);
	//pxprint(x - 2, y - 2, s);
	setcolor(color);
	return pxprint(x, y, s, shake_amp, 0, pimg);
}
RECT pxprint(int x, int y, long unicode_id, int shake_amp = 0, DWORD dwInternalDelayMs = 0, PIMAGE pimg = nullptr) {
	XCPT_L
	color_t clr = getcolor();
	int sz = textheight('%');
	int x_shake = Percent(PXPRINT_SHAKE_PROB) ? Choice({ -shake_amp, shake_amp }) : 0;
	int y_shake = Percent(PXPRINT_SHAKE_PROB) ? Choice({ -shake_amp, shake_amp }) : 0;
	PIMAGE img = AttainChnImage(unicode_id, clr);
	_resize(img, sz, sz);	//重写后的可用的函数
	putimage_alphatransparent(pimg, img, x+x_shake, y+y_shake, BLACK, g_px_alpha);
	RECT rt;
	rt.left = x;
	rt.right = x + sz;
	rt.top = y;
	rt.bottom = y + sz;
	return rt;	//返回矩形
	XCPT_R
}
RECT pxprint_instant(int x, int y, long unicode_id, int realSize, int shake_amp = 0, DWORD dwInternalDelayMs = 0, PIMAGE pimg = nullptr) {
	XCPT_L	//即时版本
	color_t clr = getcolor();
	int sz = textheight('%');
	int x_shake = Percent(PXPRINT_SHAKE_PROB) ? Choice({ -shake_amp, shake_amp }) : 0;
	int y_shake = Percent(PXPRINT_SHAKE_PROB) ? Choice({ -shake_amp, shake_amp }) : 0;
	PIMAGE img = AttainChnImageInstant(unicode_id, clr, realSize);
	_resize(img, sz, sz);	//重写后的可用的函数
	putimage_alphatransparent(pimg, img, x+x_shake, y+y_shake, BLACK, g_px_alpha);
	delimage(img);	//necessary
	RECT rt;
	rt.left = x;
	rt.right = x + sz;
	rt.top = y;
	rt.bottom = y + sz;
	return rt;	//返回矩形
	XCPT_R
}
#define PXPRINT_MAXCNT 1024
RECT pxprintf(int x, int y, int shake_amp, DWORD dwInternalDelayMs, const char* _szFormat, ...)
{
	va_list aptr;
	char buffer[PXPRINT_MAXCNT]{ 0 };
	va_start(aptr, _szFormat);
	vsprintf(buffer, _szFormat, aptr);
	va_end(aptr);
	return pxprint(x, y, buffer, shake_amp, dwInternalDelayMs);
}
#define _OFFSET_X 0
RECT CJZAPI midpxprint(const string& strText, int y, int _left=0, int _right=uix, int shake_amp = 0, DWORD dwInternalDelayMs=0, PIMAGE pimg = nullptr) {
	//自定义 y
	int fy=textheight('1');	//font height
	int stot_x = textwidth(strText.c_str());
	int x=_left+(_right-_left)/2-stot_x/2 + _OFFSET_X;
	pxprint(x,y,strText, shake_amp, dwInternalDelayMs, pimg);
	RECT rt;
	rt.left = x;
	rt.right = x+stot_x;
	rt.top = y;
	rt.bottom = y+fy;
	return rt;	//返回矩形
}
RECT CJZAPI midpxprint(long unicode_id, int y, int _left = 0, int _right = uix, int shake_amp = 0, DWORD dwInternalDelayMs = 0) {
	//自定义 y
	int fy = textheight('1');	//font height
	int stot_x = fy;	//<!>
	int x = _left + (_right - _left) / 2 - stot_x / 2 + _OFFSET_X;
	pxprint(x, y, unicode_id, shake_amp, dwInternalDelayMs);
	RECT rt;
	rt.left = x;
	rt.right = x + stot_x;
	rt.top = y;
	rt.bottom = y + fy;
	return rt;	//返回矩形
}
RECT midpxprint_shadow(const string& s, int y, int _left = 0, int _right = uix, int shake_amp = 0) {
	auto color = getcolor();
	setcolor(EGERGB(1, 1, 1));
	midpxprint(s, y + 2,  _left+2, _right+2, shake_amp);
	setcolor(color);
	return midpxprint(s, y, _left, _right, shake_amp);
}
#define WVC_AMP 12
#define WVC_OMEGA 13.0
#define WVC_PHASE0 0
clock_t lastWvBeg=0;
inline color_t WaveColor(color_t originClr) {	//originClr将成为最大值
	//闪烁的颜色 赋予游戏文字灵性
	short val = WVC_AMP * sin(WVC_OMEGA*((clock()-lastWvBeg)/1000.0)+WVC_PHASE0) - WVC_AMP*2;
	short r=EGEGET_R(originClr)+val,g=EGEGET_G(originClr)+val,b=EGEGET_B(originClr)+val;
	ClampA<short>(r,2,255);
	ClampA<short>(g,2,255);
	ClampA<short>(b,2,255);
	return EGERGB(r,g,b);
}
#define setwcolor(clr) setcolor(WaveColor(clr))
inline void CJZAPI sidebar(int left,int top,int right,int bottom,PIMAGE pimg=NULL) {	//带边框的矩形
	bar(left,top,right,bottom,pimg);
	rectangle(left,top,right,bottom,pimg);
}

string GetDVDir() {
	string ownpath = _pgmptr;
	bool suc=false;
	int b=0;
	int i=1;
	for(i=ownpath.size()-1; i>=0; i--) {
		if(ownpath.at(i) == '\\') {
			if(b == 0)	b++;
			else {
				suc = true;
				break;
			}
		}
	}
	if(!suc)
		return "\\";	//没办法的情况
	//e.g.  E:\DarkVoxel\Release\DarkVoxel.exe
	string dvpath = ownpath.substr(0,i+1);	//including backslash
	return dvpath;
}
inline string GetLocalDir() {	//有斜杠
	DV_DIR = GetDVDir();
	return (DV_DIR + "Save\\");
}
inline void _GetLocalDir() {
	LOCAL_SAVE_DIR = GetLocalDir();
	LOCAL_DATA_DIR = DV_DIR+"Data\\";
}
inline void _GetTheDirs() {
	_GetLocalDir();
}

#define CPlayerLog(...)
class CPlayer
{
public:
	CPlayer();
	~CPlayer();
public:
	BOOL Open(LPCTSTR lpFileName);//打开文件  
	BOOL Play(DWORD dwPos = 0);//播放  
	BOOL Close();//关闭  
	BOOL Stop();//停止  
	BOOL Pause();//暂停  
	BOOL Continue();//继续  
	BOOL SetVolume(DWORD dSize);
	BOOL SetPos(DWORD dwPos);
	BOOL SetAudio(bool AudioOff);
	BOOL SetSpeed(DWORD Speed);
	DWORD GetLength();//获取歌曲长度  
	BOOL GetError(LPTSTR pszText, UINT cchText);
	DWORD GetPos();
	void SetWindowHwnd(HWND hWnd);//设置主窗口句柄  
private:
	MCI_OPEN_PARMS m_ciOpen;//打开设备参数  
	HWND m_hWnd;//主窗口句柄  
	DWORD m_dwFrom;//播放起始点
	MCIERROR m_dwError;
};



//#include "stdafx.h"
//#include "Player.h"
#include <digitalv.h>

#pragma comment(lib, "Winmm.lib")
#define _USER_MCI_COMMAND


CPlayer::CPlayer()
	: m_hWnd(nullptr)
	, m_dwFrom(0)
	, m_dwError(0)
{
	ZeroMemory(&m_ciOpen, sizeof(MCI_OPEN_PARMS));
}


CPlayer::~CPlayer()
{
	if (m_ciOpen.wDeviceID)///如果有打开的MCI设备就关闭  
		Close();
}

/// 打开设备
BOOL CPlayer::Open(LPCTSTR lpFileName)
{
	CPlayerLog(__func__);
	TCHAR lpszShortPath[MAX_PATH] = { 0 };
	GetShortPathName(lpFileName, lpszShortPath, _countof(lpszShortPath));// 获得短路经
#ifdef _USER_MCI_COMMAND
	if (m_ciOpen.wDeviceID)///如果有打开的MCI设备就关闭  
		Close();
	m_ciOpen.dwCallback = (DWORD) &(m_hWnd);
	m_ciOpen.lpstrDeviceType = NULL;
	m_ciOpen.lpstrElementName = lpszShortPath;///播放文件路径
	m_ciOpen.lpstrAlias = _T("MusicAlias");
	if (m_dwError = mciSendCommand(0, MCI_OPEN, MCI_OPEN_ELEMENT, (DWORD) &(m_ciOpen)))///打开设备时，设备号为0
	{
		return FALSE;
	}
	m_dwFrom = 0;
	return TRUE;
#else
	mciSendString(_T("Stop MusicAlias"), nullptr, 0, m_hWnd);
	TCHAR lpstrCommand[MAX_PATH * 2] = {};
	_stprintf_s(lpstrCommand, _T("Open %s Alias MusicAlias"), lpszShortPath);// MusicAlias 是设备别名
	return !(m_dwError = mciSendString(lpstrCommand, nullptr, 0, m_hWnd));
#endif
}

/// 播放音乐  
BOOL CPlayer::Play(DWORD dwPos)
{
	CPlayerLog(__func__);
	m_dwFrom = dwPos;
#ifdef _USER_MCI_COMMAND
	MCI_PLAY_PARMS mciplayparms;///播放参数结构  	
	DWORD cdlen = GetLength();///得到文件大小  
	mciplayparms.dwCallback = (DWORD) &(m_hWnd);
	mciplayparms.dwFrom = m_dwFrom;///设置起始位置  
	mciplayparms.dwTo = cdlen;///设置终止位置  
	if (m_ciOpen.wDeviceID != 0)///判断是否打开文件  
		return !(m_dwError = mciSendCommand(m_ciOpen.wDeviceID, MCI_PLAY, MCI_TO | MCI_FROM, (DWORD) &(mciplayparms)));
	return FALSE;
#else
	return !(m_dwError = mciSendString(_T("Play MusicAlias"), nullptr, 0, m_hWnd));
#endif
}

/// 执行MCI_CLOSE操作，关闭MCI设备  
BOOL CPlayer::Close()
{
	CPlayerLog(__func__);
#ifdef _USER_MCI_COMMAND
	if (m_ciOpen.wDeviceID)
		return !(m_dwError = mciSendCommand(m_ciOpen.wDeviceID, MCI_CLOSE, 0, 0));
	return FALSE;
#else
	return !(m_dwError = mciSendString(_T("Close MusicAlias"), nullptr, 0, m_hWnd));
#endif
}

/// 执行MCI_STOP操作，停止播放音乐  
BOOL CPlayer::Stop()
{
	CPlayerLog(__func__);
#ifdef _USER_MCI_COMMAND
	if (m_ciOpen.wDeviceID)
	{
		if (0 == mciSendCommand(m_ciOpen.wDeviceID, MCI_STOP, 0, 0))
			return !(m_dwError = mciSendCommand(m_ciOpen.wDeviceID, MCI_SEEK, MCI_SEEK_TO_START, 0));///把播放位置设定为音乐文件的开头（下一次播放操作从文件开头位置开始）  
	}
	m_dwFrom = 0;
	return FALSE;
#else
	return !(m_dwError = mciSendString(_T("Stop MusicAlias"), nullptr, 0, m_hWnd));
#endif
}

/// 执行MCI_PAUSE操作，暂停播放音乐 
BOOL CPlayer::Pause()
{
	CPlayerLog(__func__);
#ifdef _USER_MCI_COMMAND
	if (m_ciOpen.wDeviceID)
	{
		DWORD dwsf = GetPos();
		m_dwFrom = MCI_MAKE_MSF(MCI_MSF_MINUTE(dwsf), MCI_MSF_SECOND(dwsf), MCI_MSF_FRAME(dwsf));
		return !(m_dwError = mciSendCommand(m_ciOpen.wDeviceID, MCI_PAUSE, 0, 0));
	}
	return FALSE;
#else
	return !(m_dwError = mciSendString(_T("Pause MusicAlias"), nullptr, 0, m_hWnd));
#endif
}

BOOL CPlayer::Continue()
{
	CPlayerLog(__func__);
#ifdef _USER_MCI_COMMAND
	MCI_PLAY_PARMS mciplayparms;///播放参数结构  	
	DWORD cdlen = GetLength();///得到文件大小  
	mciplayparms.dwCallback = (DWORD) &(m_hWnd);
	mciplayparms.dwTo = cdlen;///设置终止位置  
	if (m_ciOpen.wDeviceID != 0)///判断是否打开文件  
		return !(m_dwError = mciSendCommand(m_ciOpen.wDeviceID, MCI_PLAY, MCI_TO, (DWORD) &(mciplayparms)));
	return FALSE;
#else
	return !(m_dwError = mciSendString(_T("Play MusicAlias"), nullptr, 0, m_hWnd));
#endif
}

/// 得到文件大小  
DWORD CPlayer::GetLength()
{
	CPlayerLog(__func__);
#ifdef _USER_MCI_COMMAND
	MCI_STATUS_PARMS mcistatusparms;
	mcistatusparms.dwCallback = (DWORD) &(m_hWnd);
	mcistatusparms.dwItem = MCI_STATUS_LENGTH;
	mcistatusparms.dwReturn = 0;
	m_dwError = mciSendCommand(m_ciOpen.wDeviceID, MCI_STATUS, MCI_STATUS_ITEM, (DWORD) &(mcistatusparms));
	return mcistatusparms.dwReturn;/// 单位是毫秒 ms
#else
	TCHAR sPosition[256];
	DWORD lLength;
	mciSendString(_T("Status MusicAlias length"), sPosition, 255, m_hWnd);
	lLength = _tcstol(sPosition, nullptr, 10);
	return lLength;/// 单位是毫秒 ms
#endif
}

/// 得到文件播放位置
DWORD CPlayer::GetPos()
{
	CPlayerLog(__func__);
#ifdef _USER_MCI_COMMAND
	MCI_STATUS_PARMS mcistatusparms;
	mcistatusparms.dwCallback = (DWORD) &(m_hWnd);
	mcistatusparms.dwItem = MCI_STATUS_POSITION;
	mcistatusparms.dwReturn = 0;
	m_dwError = mciSendCommand(m_ciOpen.wDeviceID, MCI_STATUS, MCI_STATUS_ITEM, (DWORD) &(mcistatusparms));
	return mcistatusparms.dwReturn;
#else
	TCHAR sPosition[256];
	DWORD lLength;
	mciSendString(_T("Status MusicAlias position"), sPosition, 255, m_hWnd);
	lLength = _tcstol(sPosition, nullptr, 10);
	return lLength;/// 单位是毫秒 ms
#endif
}

void CPlayer::SetWindowHwnd(HWND hWnd)
{
	CPlayerLog(__func__);
	m_hWnd = hWnd;
}

/// 设置音量 0-100，百分比
BOOL CPlayer::SetVolume(DWORD dSize)
{
	CPlayerLog(__func__);
#ifdef _USER_MCI_COMMAND
	MCI_DGV_SETAUDIO_PARMS mciSetAudioPara;
	mciSetAudioPara.dwCallback = (DWORD) &(m_hWnd);
	mciSetAudioPara.dwItem = MCI_DGV_SETAUDIO_VOLUME;
	mciSetAudioPara.dwValue = dSize * 10; ///此处就是音量大小 (0--1000)
	return !(m_dwError = mciSendCommand(m_ciOpen.wDeviceID, MCI_SETAUDIO, MCI_DGV_SETAUDIO_VALUE | MCI_DGV_SETAUDIO_ITEM, (DWORD)&(mciSetAudioPara)));
#else
	TCHAR lpstrCommand[32] = {};
	_stprintf_s(lpstrCommand, _T("setaudio MusicAlias volume to %ld"), dSize);
	return !(m_dwError = mciSendString(lpstrCommand, nullptr, 0, m_hWnd));
#endif
}

/// 设置进度
BOOL CPlayer::SetPos(DWORD dwPos)
{
	CPlayerLog(__func__);
	m_dwFrom = dwPos;
#ifdef _USER_MCI_COMMAND
	DWORD dwLength = GetLength();///得到文件大小
	MCI_SEEK_PARMS Seek;
	Seek.dwCallback = (DWORD)&(m_hWnd);
	Seek.dwTo = dwPos;
	DWORD dwFlags = MCI_WAIT;
	if (dwPos > 0 && dwPos < dwLength)
		return !(m_dwError = mciSendCommand(m_ciOpen.wDeviceID, MCI_SEEK, MCI_WAIT | MCI_TO, (DWORD) &(Seek)));
	else if (dwPos >= dwLength)
		return !(m_dwError = mciSendCommand(m_ciOpen.wDeviceID, MCI_SEEK, MCI_WAIT | MCI_SEEK_TO_END, 0));
	return !(m_dwError = mciSendCommand(m_ciOpen.wDeviceID, MCI_SEEK, MCI_WAIT | MCI_SEEK_TO_START, 0));
#else
	TCHAR lpstrCommand[32] = {};
	_stprintf_s(lpstrCommand, _T("seek MusicAlias to %ld"), dwPos);
	return !(m_dwError = mciSendString(lpstrCommand, nullptr, 0, m_hWnd));
#endif
}

/// 静音True为静音，FALSE为取消静音
BOOL CPlayer::SetAudio(bool AudioOff)
{
	CPlayerLog(__func__);
#ifdef _USER_MCI_COMMAND
	MCI_SET_PARMS setAudio;
	setAudio.dwCallback = (DWORD)&(m_hWnd);
	setAudio.dwTimeFormat = MCI_FORMAT_MILLISECONDS;
	setAudio.dwAudio = MCI_SET_AUDIO_ALL;
	DWORD dwFlags = MCI_SET_AUDIO;
	if (AudioOff)
		dwFlags |= MCI_SET_ON;
	else
		dwFlags |= MCI_SET_OFF;
	return !(m_dwError = mciSendCommand(m_ciOpen.wDeviceID, MCI_SET, dwFlags, (DWORD) &(setAudio)));
#else
	if (AudioOff)
		return !(m_dwError = mciSendString(_T("setaudio MusicAlias off"), nullptr, 0, m_hWnd));
	else
		return !(m_dwError = mciSendString(_T("setaudio MusicAlias on"), nullptr, 0, m_hWnd));
#endif
}

/// 设置播放速度1-200，百分比
BOOL CPlayer::SetSpeed(DWORD Speed)
{
	CPlayerLog(__func__);
#ifdef _USER_MCI_COMMAND
	MCI_DGV_SET_PARMS setSpeed;
	setSpeed.dwCallback = (DWORD)&(m_hWnd);
	//setSpeed.dwTimeFormat = MCI_FORMAT_MILLISECONDS;
	//setSpeed.dwAudio = MCI_SET_AUDIO_ALL;
	//setSpeed.dwFileFormat = 0;
	setSpeed.dwSpeed = Speed * 10;/// 设置播放速度1-2000,1000为正常速度
	DWORD dwFlags = MCI_DGV_SET_SPEED;
	return !(m_dwError = mciSendCommand(m_ciOpen.wDeviceID, MCI_SET, dwFlags, (DWORD) &(setSpeed)));
#else
	TCHAR lpstrCommand[32] = {};
	_stprintf_s(lpstrCommand, _T("set MusicAlias speed %ld"), Speed);
	return !(m_dwError = mciSendString(lpstrCommand, nullptr, 0, m_hWnd));
#endif
}

/// 获得错误描述
BOOL CPlayer::GetError(LPTSTR pszText, UINT cchText)
{
	return mciGetErrorString(m_dwError, pszText, cchText);
}
//from https://blog.csdn.net/u010682281/article/details/66473313
//↑ 请支持作者！ 

#define AUDIO_DIR (GetDVDir()+"Audio\\")
bool g_playing = false;

struct SoundTimeRecord
{
	string name;	//名称				音乐也挤一挤
	DWORD ms;	//毫秒数
};
vector<SoundTimeRecord> g_sndtmrec;
#define STREC_PATH (AUDIO_DIR + "soundTimeRecord.dat")
void ReadSoundTimeRecords()
{
	DebugLog("ReadSoundTimeRecords: 读取音效记录的时长...");
	if (!g_sndtmrec.empty())	g_sndtmrec.clear();
	if (!ExistFile(STREC_PATH))
	{
		ErrorLogTip(NULL, "音效时长文件缺失:" + STREC_PATH, "ReadSoundTimeRecords");
		return;
	}
	vector<string> lines = ReadFileLines(STREC_PATH);
	int i = 1;
	while (i < lines.size())
	{
		string line = lines.at(i);
		stringstream ss;
		ss << line;
		SoundTimeRecord ele;
		ss >> ele.name;
		ss >> ele.ms;
		g_sndtmrec.push_back(ele);
		++i;
	}
}
DWORD GetSoundTime(const string& name)
{	//ms
	for (int i = 0; i < g_sndtmrec.size(); ++i)
		if (g_sndtmrec.at(i).name == name)
			return g_sndtmrec.at(i).ms;
	return 0;
}
#define SNDTRG_OFF '0'		//关闭音效
#define SNDTRG_ONCE '1'		//启动后立即变成'0'
#define SNDTRG_REPEAT 'R'	//直到手动改为'0'
vector<char> g_sndTriggers;
volatile int g_sndProc_total = 0;
string g_sndProc_name = "";
#define SND_SUFFIX ".mp3"
#define SND_MIN_PLAYING_TIME 20
#define SND_SPEED 100
DWORD SoundProc(LPVOID args)
{
	string name = g_sndProc_name;
	int index = g_sndProc_total;	//副本
	//DebugLog("SoundProc:  name=" + name + " index=" + ToString(index) + " total=" + ToString(g_sndProc_total));
	CPlayer cpr;
	string path = AUDIO_DIR + name + SND_SUFFIX;
	BOOL bRet = cpr.Open(path.c_str());
	cpr.SetSpeed(SND_SPEED);
	g_sndProc_total++;
	if (!bRet)
	{
		ErrorLogTip(NULL, "无法打开" + path + "音效文件,name=" + name, "SoundProc");
		return -1;
	}
	DWORD dwLen = cpr.GetLength();	
	//DWORD dwLen = GetSoundTime(name)/*GetMPEGTime(path)*1000*//*cpr.GetLength()*/;
	DebugLog("SoundProc: 名为" + name + "的音效成功被打开 len=" + ToString(dwLen)+" g_playing="+ToString(g_playing));
	while (is_run())
	{
		if (g_sndTriggers.at(index) == SNDTRG_ONCE)
		{	//触发音效
			g_sndTriggers.at(index) = SNDTRG_OFF;	//关闭触发器
			//DebugLog("开始播放" + name);
			cpr.Play(0);
			clock_t start = clock();
			while ((clock() - start) / (double)CLOCKS_PER_SEC * 1000 < dwLen)
			{
				if (g_sndTriggers.at(index) == SNDTRG_ONCE
					&& (clock() - start) / (double)CLOCKS_PER_SEC * 1000 >= SND_MIN_PLAYING_TIME	//最小播放长度
					)
				{
					cpr.Stop();
					break;	//立即重新播放
				}
				else if (!is_run())
				{
					goto _end;
				}
			}
			//cpr.Stop();
		}
		else if (g_sndTriggers.at(index) == SNDTRG_REPEAT)
		{
			cpr.Play(0);
			while (is_run() && g_sndTriggers.at(index) == SNDTRG_REPEAT)
			{
				cpr.SetPos(0);
				cpr.Play();
				clock_t start = clock();
				while ((clock() - start) / (double)CLOCKS_PER_SEC * 1000 < dwLen)
				{
					/*if (g_sndTriggers.at(index) == SNDTRG_OFF)
					{
						cpr.Stop();
						break;
					}
					else */if (!is_run())
					{
						goto _end;
					}
				}
				//cpr.Stop();
			}
			g_sndTriggers.at(index) = SNDTRG_OFF;
		}
		api_sleep(2);
	}
_end:
	cpr.Stop();
	cpr.Close();
	return 0;
}
#define MUS_SUFFIX ".mp3"
#define MUS_VOLUME 30
string g_musName;	//不含mus_
#define MUSTRG_OFF 0
#define MUSTRG_ON 1
#define MUSTRG_PAUSED 2
volatile int    g_musTrigger = MUSTRG_ON;	//音乐触发器
//一次只能播放一首音乐
DWORD MusicProc(LPVOID args)
{	//单次播放
	CPlayer cpr;
	BOOL bRet = TRUE;
	DWORD dwLen = 0;
	string path = AUDIO_DIR + g_musName + MUS_SUFFIX;
	bRet = cpr.Open((path).c_str());
	if (!bRet)
	{
		ErrorLogTip(NULL, "无法打开音乐" + g_musName, "MusicProc");
		return -1;
	}
	//dwLen = GetMPEGTime(path) * 1000 + 1000;	//防止提前咔掉
	//dwLen = GetSoundTime(g_musName);
	dwLen = cpr.GetLength();
	//DebugLog("dwLen=" + ToString(dwLen));
	clock_t start = clock();
	//cpr.SetVolume(MUS_VOLUME);
	cpr.Play();
	g_musTrigger = MUSTRG_ON;
	while ((clock() - start) / (double)CLOCKS_PER_SEC * 1000 < dwLen
		 && is_run())
	{
		if (g_musTrigger == MUSTRG_OFF)	//立即停止
			break;
		if (g_musTrigger == MUSTRG_PAUSED)	//暂停
		{
			cpr.Pause();
			clock_t pause_start = clock();
			while (g_musTrigger == MUSTRG_PAUSED)
				api_sleep(1);
			if (g_musTrigger == MUSTRG_ON)
				cpr.Continue();
			start += clock() - pause_start;
		}
		api_sleep(1);
	}
	g_musTrigger = MUSTRG_OFF;
	cpr.Stop();
	cpr.Close();
	return 1;
}
DWORD MusicLoopProc(LPVOID args)
{	//循环播放
	XCPT_L
	CPlayer cpr;
	BOOL bRet = TRUE;
	DWORD dwLen = 0;
	string path = AUDIO_DIR + g_musName + MUS_SUFFIX;
	bRet = cpr.Open((path).c_str());
	if (!bRet)
	{
		ErrorLogTip(NULL, "无法打开音乐" + g_musName, "MusicLoopProc");
		return -1;
	}
	//dwLen = GetMPEGTime(path) * 1000
	//	+ 1000;	//防止提前咔掉;
	dwLen = cpr.GetLength();
	//DebugLog("Path="+path+" dwLen=" + ToString(dwLen));
	clock_t start;
	g_musTrigger = MUSTRG_ON;
	while (is_run() && g_musTrigger == MUSTRG_ON)
	{
		start = clock();
		cpr.Play(0);
		while ((clock() - start) / (double)CLOCKS_PER_SEC * 1000 < dwLen
			&& is_run())
		{
			if (g_musTrigger == MUSTRG_OFF)	//立即停止
				break;
			if (g_musTrigger == MUSTRG_PAUSED)	//暂停
			{
				cpr.Pause();
				clock_t pause_start = clock();
				while (g_musTrigger == MUSTRG_PAUSED)
					api_sleep(1);
				if (g_musTrigger == MUSTRG_ON)
					cpr.Continue();
				start += clock() - pause_start;
			}
			api_sleep(1);
		}
		cpr.Pause();
	}
	g_musTrigger = MUSTRG_OFF;
	cpr.Close();
	return 1;
	XCPT_R
		return 0;
}

struct OPTIONS {	//游戏配置
	bool music_switch;
	bool sound_switch;
	bool blocktag_switch;
	bool entitytag_switch;
	bool itemtag_switch;
	bool show_weather;
	bool view_side_fx;
	bool show_debug_info;
	bool durability_bar;

	bool player_mutter;
	bool mob_spawning;
	bool death_drop;
	bool creature_drop;

	string option_ver;

	OPTIONS() {
		Init();
	}
	void Init() {
		//每次运行时的初始化
		music_switch = true;
		sound_switch = true;
		blocktag_switch = false;
		itemtag_switch = true;
		entitytag_switch = true;
		show_debug_info = false;
		show_weather = true;
		durability_bar = false;
		view_side_fx = true;

		option_ver = "";
		player_mutter = true;
		mob_spawning = true;
		death_drop = true;
		creature_drop = true;
	}
	void FirstRunInit(void) {
		//游戏第一次运行时的初始化
		music_switch = true;
		sound_switch = true;
		itemtag_switch = true;
		entitytag_switch = true;
		blocktag_switch = true;
		show_debug_info = false;
		show_weather = true;
		durability_bar = false;
		view_side_fx = true;

		option_ver = CURRENT_VERSION;
		player_mutter = true;
		mob_spawning = true;
		death_drop = true;
		creature_drop = true;
	}
} options;

struct SndItem
{
	int    index;
	string name;
	HANDLE hThread;
};
class SoundManager	//音效管理器
{
private:
	vector<SndItem> m_sndItems;

	int m_curMusic_index;
	vector<string> m_musNames;
	HANDLE hMusThread;

public:
	SoundManager()
	{
		m_curMusic_index = -1;
	}
	~SoundManager()
	{
		for (const auto& ele : m_sndItems)
		{
			CloseHandle(ele.hThread);
		}
		m_sndItems.clear();
		if (g_musTrigger == MUSTRG_ON)
			CloseHandle(hMusThread);
		m_musNames.clear();
		g_sndTriggers.clear();
	}
	bool PrepareSound(const string& name)
	{
		g_sndTriggers.push_back(SNDTRG_OFF);
		SndItem item;
		item.name = name;
		item.index = m_sndItems.size();
		//DebugLog("SoundManager::PrepareSound:  name=" + name + " index=" + ToString(item.index) + " total=" + ToString(g_sndProc_total));
		while (item.index > g_sndProc_total)
		{
			//Sleep(1);
		}
		m_sndItems.push_back(item);
		g_sndProc_name = name;
		m_sndItems.at(m_sndItems.size() - 1).hThread =
			CreateThread((LPTHREAD_START_ROUTINE)SoundProc, nullptr);
		return true;
	}
	SndItem& GetSound(const string& name)
	{
		for (int i = 0; i < m_sndItems.size(); ++i)
		{
			if (m_sndItems.at(i).name == name)
				return m_sndItems.at(i);
		}
		ErrorLogTip(NULL, "无法获取名称为" + name + "的音效", "SoundManager::GetSound");
		abort();
		return m_sndItems.at(0);
	}
	inline void PlaySound(const string& name)
	{
		if (!options.sound_switch)	return;
		//DebugLog("SoundManager::PlaySound: 播放音效" + name);
		g_sndTriggers.at(GetSound(name).index) = SNDTRG_ONCE;	//一触即发
	}
	inline void LoopPlaySound(const string& name)
	{
		if (!options.sound_switch)	return;
		g_sndTriggers.at(GetSound(name).index) = SNDTRG_REPEAT;
	}
	inline void StopPlaySound(const string& name)
	{	//仅对循环播放的音效有效
		g_sndTriggers.at(GetSound(name).index) = SNDTRG_OFF;
	}
	inline void StopPlayAllSound(void)
	{	//仅对循环播放的音效有效
		for (size_t i = 0; i < g_sndTriggers.size(); ++i)
			g_sndTriggers.at(i) = SNDTRG_OFF;
	}
	void PlayMusic(const string& name)
	{
		if (!options.music_switch)	return;
		g_musName = name;
		g_musTrigger = MUSTRG_ON;
		hMusThread = CreateThread((LPTHREAD_START_ROUTINE)MusicProc);
	}
	void LoopPlayMusic(const string& name)
	{
		if (!options.music_switch)	return;
		g_musName = name;
		g_musTrigger = MUSTRG_ON;
		hMusThread = CreateThread((LPTHREAD_START_ROUTINE)MusicLoopProc);
	}
	void StopPlayMusic(void)
	{
		g_musName = "";
		g_musTrigger = MUSTRG_OFF;
	}
	inline void PauseMusic(void)
	{
		g_musTrigger = MUSTRG_PAUSED;
	}
	inline void ContinueMusic(void)
	{
		if (!options.music_switch)	return;
		g_musTrigger = MUSTRG_ON;
	}
};
SoundManager g_sndmgr;
#define SND(name)	g_sndmgr.PlaySoundA(name)
#define SNDLOOP(name)	g_sndmgr.LoopPlaySound(name)
#define STOPSNDLOOP(name)	g_sndmgr.StopPlaySound(name)
#define MUS(name)	g_sndmgr.PlayMusic(name)
#define MUSLOOP(name)	g_sndmgr.LoopPlayMusic(name)
#define STOPMUS(...)	g_sndmgr.StopPlayMusic()
#define PAUSEMUS(...)	g_sndmgr.PauseMusic()
#define CONTINUEMUS(...) g_sndmgr.ContinueMusic()
void PrepareSounds()
{
	ReadSoundTimeRecords();
	vector<string> soundFiles = GetDirFilesR(AUDIO_DIR, "*.mp3");	//先遍历搜寻音效文件
	DebugLog("PrepareSounds: 共在" + AUDIO_DIR + "找到" + ToString(soundFiles.size()) + "个音效文件.");
	g_sndProc_total = 0;
	for (short i = 0; i < soundFiles.size(); ++i)
	{
		soundFiles.at(i) = strxtail(soundFiles.at(i), 4);	//去头除尾
		g_sndmgr.PrepareSound(soundFiles.at(i));	//准备起来
	}
}

#define OPTIONS_PATH (DV_DIR+"options.ini")
#define PLAYERDATA_PATH (LOCAL_SAVE_DIR+"PLAYER.DAT")
#define WORLDDATA_PATH (LOCAL_SAVE_DIR+"WORLD.DAT")
#define BLOCKDATA_PATH (LOCAL_DATA_DIR+"BlockData.csv")
#define ITEMDATA_PATH (LOCAL_DATA_DIR+"ItemData.csv")
#define BIOMEDATA_PATH (LOCAL_DATA_DIR+"BiomeData.csv")
#define EFFECTDATA_PATH (LOCAL_DATA_DIR+"EffectData.csv")
#define ENTITYDATA_PATH (LOCAL_DATA_DIR+"EntityData.csv")
#define DPDATA_PATH (LOCAL_DATA_DIR+"DamagePointData.csv")
#define BDJSONDATA_PATH (LOCAL_DATA_DIR+"BlockDefaultJSONData.prn")
#define EDJSONDATA_PATH (LOCAL_DATA_DIR+"EntityDefaultJSONData.prn")
#define IDJSONDATA_PATH (LOCAL_DATA_DIR+"ItemDefaultJSONData.prn")
#define TIPS_PATH (LOCAL_DATA_DIR+"GameTips.prn")
#define RECIPE_PATH (LOCAL_DATA_DIR+"Recipes.prn")
#define ACHVDATA_PATH (LOCAL_DATA_DIR+"Achievements.csv")
#define CRAFTTYPEDATA_PATH (LOCAL_DATA_DIR+"CraftTypeData.prn")
///////////////////////////////////////////////////////////////////

void ReadOptions();
void ReadItemData();
void ReadCraftTypeData();
void ReadBlockData();
void ReadBiomeData();
void ReadDPData();
void ReadEffectData();
void ReadEntityData();
void ReadAchievementData();
void ReadBlockDefaultJSONData();
void ReadItemDefaultJSONData();
void ReadRecipes();
void CloseContainer();
void Save();
void Exit();
void EnterOptions();
void SaveOptions();
long AttainNewUUID(void);
long GetPlayerShapeID();
color_t GetPlayerColor();
long GetPlayerX();
long GetPlayerY();
short GetPlayerHp();
short GetPlayerMp();
short GetPlayerSp();
short GetPlayerMaxHp();
short GetPlayerMaxMp();
short GetPlayerMaxSp();
string GetPlayerName();
color_t ParseColor(const string& s);
void DropContentItems(long x, long y, const vector<string>& content);
template <typename _T>
struct Weight {
	short weight;
	_T value;

	Weight() {
		weight = 1;
	}
};
typedef USHORT DIR,*PDIR;	//方向
#define UP 0x01
#define DIR_FIRST UP
#define RIGHTUP 0x02
#define RIGHT 0x03
#define RIGHTDOWN 0x04
#define DOWN 0x05
#define LEFTDOWN 0x06
#define LEFT 0x07
#define LEFTUP 0x08
#define DIR_LAST LEFTUP
bool OuttaWorld(const long& _x, const long& _y, bool onlyLoaded = true);
bool OuttaWorld(long x, long y, DIR dir);
bool IsBarrier(long ox, long oy, DIR dir);
inline double GetDirRadian(DIR dir)
{	//获取方向弧度rad
	switch (dir)
	{
	case UP: {
		return 0;
		break;
	}
	case LEFTUP: {
		return (PI / 4);
		break;
	}
	case LEFT: {
		return PI / 2;
		break;
	}
	case LEFTDOWN: {
		return (PI / 4 * 3);
		break;
	}
	case DOWN: {
		return PI;
		break;
	}
	case RIGHTDOWN: {
		return (PI*1.25);
		break;
	}
	case RIGHT: {
		return (PI*1.5);
		break;
	}
	case RIGHTUP: {
		return (PI*1.75);
		break;
	}
	default: {
		ErrorLogTip(NULL, "无效的方向", "GetDirRadian");
		return 0;
		break;
	}
	}
}
inline DIR RandomDir4(void) {
	return Choice({UP,RIGHT,DOWN,LEFT});
}
inline DIR RandomDir8(void) {
	return DIR(RandomRange(1,8));
}
inline DIR OppoDir(DIR dir)
{
	return (dir + 4) % 8;
}
DIR GetDir8(long dx, long dy)
{
	DIR adir=RIGHT;
	double k = abs(dy) / (double)abs(dx);
	if (dx > 0)
	{	//右
		if (k <= tan(PI / 8.0))
		{
			adir = RIGHT;
		}
		else if (k <= tan(PI * 3.0 / 8.0))
		{
			adir = (dy < 0 ? RIGHTUP : RIGHTDOWN);
		}
		else {
			adir = (dy < 0 ? UP : DOWN);
		}
	}
	else {	//左
		if (k <= tan(PI / 8.0))
		{
			adir = LEFT;
		}
		else if (k <= tan(PI * 3.0 / 8.0))
		{
			adir = (dy < 0 ? LEFTUP : LEFTDOWN);
		}
		else {
			adir = (dy < 0 ? UP : DOWN);
		}
	}
	return adir;
}
DIR GetDir4(long dx, long dy)
{
	DIR adir=RIGHT;
	double k = abs(dy) / (double)abs(dx);
	if (dx > 0)
	{
		if (k <= 1.0)
		{
			adir = RIGHT;
		}
		else {
			adir = (dy < 0 ? UP : DOWN);
		}
	}
	else {
		if (k <= 1.0)
		{
			adir = LEFT;
		}
		else {
			adir = (dy < 0 ? UP : DOWN);
		}
	}
	return adir;
}

void DirOffsetPos(long &x, long &y, DIR dir, const char* tag=nullptr) {
	//依据方向位移坐标
	if (dir == UP)
		y--;
	else if (dir == RIGHTUP)
		x++, y--;
	else if (dir == RIGHT)
		x++;
	else if (dir == RIGHTDOWN)
		x++, y++;
	else if (dir == DOWN)
		y++;
	else if (dir == LEFTDOWN)
		x--, y++;
	else if (dir == LEFT)
		x--;
	else if (dir == LEFTUP)
		x--, y--;
	else {
		return;
	}
}

/*struct UnicodeData {
	USHORT id;
	wchar_t* wstr;
};
vector<UnicodeData> unicodedata;
wchar_t* GetUnicode(USHORT id)
{
	if (id >= unicodedata.size())
	{
		ErrorLogTip(NULL, "Unicode列表访问越界", "GetUnicode");
		return nullptr;
	}
	return unicodedata.at(id).wstr;
}
void ReadUnicodeData()
{
	XCPT_L
	if (!ExistFile(UNICODEDATA_PATH)) {
		ErrorLogTip(NULL, "Unicode符号数据丢失", "ReadUnicodeData");
		return;
	}
	DebugLog("【读取】读取Unicode符号数据：" + UNICODEDATA_PATH);
	wifstream fin(UNICODEDATA_PATH);
	while (!fin.eof())
	{
		UnicodeData ud;
		fin >> ud.id;
		fin >> ud.wstr;
		unicodedata.push_back(ud);
	}
	fin.close();
	XCPT_R
}*/
typedef USHORT BLOCK_ID, BLOCK_CLASS_ID, ENTITY_ID;
void CheckAchvDigs(BLOCK_ID id);

string UncoverValue(const string& value, const string& type){
	if(type == "" || type == "unknown") {
		ErrorLogTip(NULL,"数据类型无效:\n"+type,"UncoverValue");
		return value;
	}
	if(type == "string")
		return strxtail(strxhead(value));
	/*else if(type == "long long" || type == "double" || type == "unsigned long")
		return strxtail(value,2);*/
	else if(type == "color")	//#开头 e.g.  #0;0;0
		return strxhead(value,1);
	else
		return strxtail(value,1);
}
string GetValueType(const string& s)
{
	if(strtail(s) == "\"" && strhead(s)=="\""
	        || strtail(s) == "\'" && strhead(s) == "\'")
		return "string";
	if(strtail(s) == "]" && strhead(s)=="[")
		return "list";
	if(strtail(s) == "}" && strhead(s)=="{")
		return "json";
	if(strhead(s) == "#")
		return "color";
	if(strhead(s) != "[" && strhead(s) != "{" && strhead(s) != "\"" && strhead(s) != "\'") {
		string tail = strtail(s);
//				DebugLog("tail="+tail);
		/*if(sequ(tail,"b"))
			return "BYTE";*/
		if(sequ(tail,"s"))
			return "short";
		if(sequ(tail,"f"))
			return "float";
		if(sequ(tail,"l"))
			return "long";
	}
	return "unknown";	//未知
}

class JSON {
	private:
		vector<pair<string,string>> _data;
	public:
		JSON() {
		}
		inline JSON& operator= (const JSON& src)
		{
			this->_data = src._data;
			return *this;
		}
		void Clear() {
			_data.clear();
		}
		bool Empty()	const {
			return _data.empty();
		}
		bool HaveKey(const char* key) const {
			XCPT_L
			if(_data.empty())	return false;
			for(short i = 0; i < _data.size(); ++i)
				if(_data.at(i).first == key)
					return true;
			return false;
			XCPT_R
		}
		void ToLog(void)	const {
			DebugLog("JSON ToLog: "+ToString());
		}
		string GetValueRaw(const char* key) const {
//			TrackLog("...");
			for(short i = 0; i < _data.size(); ++i)
				if(_data.at(i).first == key)
					return _data.at(i).second;
			return "";
		}
		bool SetValueRaw(const char* key, const char* newValueRaw) {
			XCPT_L
//			TrackLog("...");
			for(short i = 0; i < _data.size(); ++i)
				if(_data.at(i).first == key) {
					_data.at(i).second = newValueRaw;
					return true;
				}
			XCPT_R
			return false;
		}
		bool SetShort(const char* key, short value) {
			if(!HaveKey(key) || GetValueType(key,"JSON::SetShort") != "short")
				return false;
			TrackLog("设置短整数");
			SetValueRaw(key, (::ToString(value)+"s").c_str());
			return true;
		}
		short GetShort(const char * key)	const
		{	//如果键不存在则报错
			if (!HaveKey(key) || GetValueType(key, "JSON::GetShort") != "short")
			{
				ErrorLogTip(NULL, "找不到短整型" + ::ToString(key), "JSON::GetShort");
				return 0;
			}
			short res = 0;
			GetSimpleValue<short>(key, &res);
			return res;
		}
		bool SetLong(const char* key, long value) {
			if (!HaveKey(key) || GetValueType(key, "JSON::SetLong") != "long")
				return false;
			TrackLog("设置长整数");
			SetValueRaw(key, (::ToString(value) + "L").c_str());
			return true;
		}
		long GetLong(const char * key)	const
		{	//如果键不存在则报错
			if (!HaveKey(key) || GetValueType(key, "JSON::GetLong") != "long")
			{
				ErrorLogTip(NULL, "找不到长整型" + ::ToString(key), "JSON::GetLong");
				return 0;
			}
			long res = 0;
			GetSimpleValue<long>(key, &res);
			return res;
		}
		bool SetFloat(const char* key, float value) {
			if (!HaveKey(key) || GetValueType(key, "JSON::SetFloat") != "float")
				return false;
			TrackLog("设置浮点数");
			SetValueRaw(key, (::ToString(value) + "f").c_str());
			return true;
		}
		float GetFloat(const char * key)	const
		{	//如果键不存在则报错
			if (!HaveKey(key) || GetValueType(key, "JSON::GetFloat") != "float")
			{
				ErrorLogTip(NULL, "找不到浮点型" + ::ToString(key), "JSON::GetFloat");
				return 0;
			}
			float res = 0;
			GetSimpleValue<float>(key, &res);
			return res;
		}
		bool SetString(const char* key, const string& value) {
			if (!HaveKey(key) || GetValueType(key, "JSON::SetString") != "string")
				return false;
			TrackLog("设置字符串");
			SetValueRaw(key, ("\"" + ::ToString(value) + "\"").c_str());
			return true;
		}
		string GetString(const char * key)	const
		{	//如果键不存在则报错
			if (!HaveKey(key) || GetValueType(key, "JSON::GetString") != "string")
			{
				ErrorLogTip(NULL, "找不到字符串" + ::ToString(key), "JSON::GetFloat");
				return 0;
			}
			string res;
			GetSimpleValue<string>(key, &res);
			return res;
		}
		bool SetColor(const char* key, color_t value) {
			if (!HaveKey(key) || GetValueType(key, "JSON::SetColor") != "color")
				return false;
			TrackLog("设置颜色值");
			SetValueRaw(key, ("#" + ::ToString(EGEGET_R(value)) + ";" + ::ToString(EGEGET_G(value)) + ";" + ::ToString(EGEGET_B(value))).c_str());
			return true;
		}
		color_t GetColor(const char * key)	const
		{	//如果键不存在则报错
			if (!HaveKey(key) || GetValueType(key, "JSON::GetColor") != "color")
			{
				ErrorLogTip(NULL, "找不到颜色值" + ::ToString(key), "JSON::GetColor");
				return 0;
			}
			color_t res;
			GetComplexValue(key, &res);
			return res;
		}
		bool SetList(const char* key, const vector<string>& _list) {
			if(!HaveKey(key) || GetValueType(key,"JSON::SetList") != "list")
				return false;
			TrackLog("设置列表");
			SetValueRaw(key, ::ToString(_list).c_str());
			return true;
		}
		vector<string> GetList(const char* key) const
		{
			if (!HaveKey(key) || GetValueType(key, "JSON::GetList") != "list")
			{
				ErrorLogTip(NULL, "找不到列表" + ::ToString(key), "JSON::GetList");
				return vector<string>();
			}
			vector<string> res;
			GetComplexValue(key, &res);
			return res;
		}
		string GetValueType(const char* key, const char* tag=nullptr) const {
			string s = GetValueRaw(key);
//			DebugLog("GetValueType tag="+::ToString(tag)+" keyraw="+s);
			if(s.empty())	return "";	//not found
			return ::GetValueType(s);
		}
		template <typename _T>
		bool GetSimpleValue(const char* key, _T* result)	const;
		template <>
		bool GetSimpleValue(const char* key, string* result) const;

		bool GetComplexValue(const char* key, void* result)	const;

		void AddItem(const string& key, const string& value) {
			XCPT_L
//			DebugLog("JSON::Additem key="+key+" value="+value );
			_data.push_back(make_pair(key,value));
			XCPT_R
		}
		bool RemoveItem(const string& key)
		{
			auto iter = begin(_data);
			for(short j = 0; j < _data.size(); ++j, ++iter)
				if(_data.at(j).first == key)
				{
					_data.erase(iter);
					return true;
				}
			return false;
		}
		string ToString(void)	const {
			string ret="{";
			for(short i = 0; i < _data.size(); ++i) {
				ret += _data.at(i).first;
				ret += ":";
				ret += _data.at(i).second;
				if(i != _data.size() - 1)
					ret += ",";
			}
			ret += "}";
			return ret;
		}
};

#define PJERR(msg) ErrorLogTip(NULL,"无效的JSON：\n"+src+"\n原因："+ToString(msg),"ParseJSON"+(tag?" tag="+ToString(tag):string("")))
JSON ParseJSON(string src, const char* tag="") {
	XCPT_L

//	DebugLog("ParseJSON len="+ToString(src.size())+" src="+src);
	if(src.empty() || strhead(src) != "{" || strtail(src) != "}") {
		PJERR("无基本结构");
		return JSON();
	}
	if(src == "{}")
		return JSON();
	JSON res;
	stack<char> brackets;
	bool quote = false, midDone = false;
	short beg=1, mid=-1;
	for(short i = 1; i < src.size(); ++i) {
		//0123456 len=7
		//{K:Val}
		//{A:123,B:"456",C:[1,2,3],D:{DEE:3}}
		char c = src.at(i);
		if(brackets.empty()) {
			if(c == ':') {
				if(midDone) {
					PJERR("含多个冒号");
					return JSON();
				}
				mid = i;
				midDone = true;
			} else if(i == src.size()-1 || c == ',') {
				string kv = src.substr(beg, i - beg);
				if(kv.empty()) {
					PJERR("出现空项");
					return JSON();
				} else if(!midDone) {
					PJERR("项内无冒号");
					return JSON();
				}
				string key = kv.substr(0, mid-beg);
				string value = kv.substr(mid-beg+1, kv.size()+beg-mid-1);
				res.AddItem(key.c_str(), value.c_str());
//				DebugLog("添加新键值 key="+key+" value="+value);
				if(res.GetValueType(key.c_str(),("ParseJSON tag="+ToString(tag)).c_str()) == "unknown") {
					PJERR("无效的值="+value);
					return JSON();
				}
				beg = i+1;
				mid = -1;
				midDone = false;
			} else if(c == '(' || c == ')') {
				PJERR("不允许出现小括号");
				return JSON();
			}
		}
		if(!quote && (c == '[' || c == '{')) {
			brackets.push(c);
		} else if(!quote && (c == ']' || c == '}')) {
			if(c == '}' && i == src.size()-1)
				continue;	//end ignore
			if(brackets.empty() || brackets.top() != c-2) {
				PJERR("括号不匹配。含多余的右半部分");
				return JSON();
			}
			brackets.pop();
		} else if(c == '"' || c == '\'') {
			if(quote) {
				if(brackets.top() != c) {
					continue;
				}
				brackets.pop();
				quote = false;
			} else {
				brackets.push(c);
				quote = true;
			}
		}
	}
	if(!brackets.empty()) {
		PJERR("括号不匹配。含多余的左半部分");
		return JSON();
	}
	return res;
	XCPT_R
}
#define PLERR(msg) ErrorLogTip(NULL,"无效的列表：\n"+src+"\n原因："+ToString(msg),"ParseList"+(tag?" tag="+ToString(tag):string("")))
vector<string> ParseList(const string& src, const char* tag="") {
	XCPT_L
	if(src.empty() || strhead(src) != "[" || strtail(src) != "]") {
		PLERR("无基本结构");
		return vector<string>();
	}
	if(src == "[]")
		return vector<string>();
	vector<string> res;
	stack<char> brackets;
	bool quote = false;
	short beg=1;
	for(short i = 1; i < src.size(); ++i) {
		char c = src.at(i);
		if(brackets.empty()) {
			if(i == src.size()-1 || c == ',') {
				string item = src.substr(beg, i - beg);
				if(item.empty()) {
					PLERR("出现空项");
					return vector<string>();
				}
				res.push_back(item);
				beg = i+1;
			} else if(c == '(' || c == ')') {
				PLERR("不允许出现小括号");
				return vector<string>();
			}
		}
		if(!quote && (c == '[' || c == '{')) {
			brackets.push(c);
		} else if(!quote && (c == ']' || c == '}')) {
			if(c == ']' && i == src.size()-1)
				continue;	//end ignore
			if(brackets.empty() || brackets.top() != c-2) {
				PLERR("括号不匹配。含多余的右半部分");
				return vector<string>();
			}
			brackets.pop();
		} else if(c == '"' || c == '\'') {
			if(quote) {
				if(brackets.top() != c) {
					continue;
				}
				brackets.pop();
				quote = false;
			} else {
				brackets.push(c);
				quote = true;
			}
		}
	}
	if(!brackets.empty()) {
		PLERR("括号不匹配。含多余的左半部分");
		return vector<string>();
	}
//	TrackLog("long list, size="+ToString(res.size()));
	return res;
	XCPT_R
}

template <typename _T>
bool JSON::GetSimpleValue(const char* key, _T* result)	const {
	//简单类型直接使用模板函数转换
	string raw = GetValueRaw(key);
	if(raw.empty()) {
		ErrorLogTip(NULL,"没有找到指定键："+::ToString(key),"JSON::GetSimpleValue");
		return false;
	}
	string type = GetValueType(key,"JSON::GetSimpleValue");
	if(sequ(type,"json") || sequ(type,"list")) {
		ErrorLogTip(NULL,"数据类型并不是简单类型 而是"+type);
		return false;
	}
	raw = UncoverValue(raw, type);
	*result = Convert<string, _T>(raw);
	return true;
}
template <>
bool JSON::GetSimpleValue(const char* key, string* result)	const {
	//简单类型直接使用模板函数转换
	string raw = GetValueRaw(key);
	if (raw.empty()) {
		ErrorLogTip(NULL, "没有找到指定键：" + ::ToString(key), "JSON::GetSimpleValue");
		return false;
	}
	string type = GetValueType(key, "JSON::GetSimpleValue");
	if (sequ(type, "json") || sequ(type, "list")) {
		ErrorLogTip(NULL, "数据类型并不是简单类型 而是" + type);
		return false;
	}
	raw = UncoverValue(raw, type);
	*result = raw;
	return true;
}
bool JSON::GetComplexValue(const char* key, void* result)	const {
	//复杂类型单独处理
	string raw = GetValueRaw(key);
	if(raw.empty()) {
		ErrorLogTip(NULL,"没有找到指定键："+::ToString(key),"JSON::GetComplexValue");
		return false;
	}
	string type = GetValueType(key,"JSON::GetComplexValue");
//	TrackLog(".");
	if(sequ(type,"json")) {
		*((JSON*)result) = ParseJSON(raw,"JSON::GetComplexValue");
		return true;
	}
	if(sequ(type,"list")) {
		*((vector<string>*)result) = ParseList(raw);
		return true;
	}
	if(sequ(type,"color")) {
		*((color_t*)result) = ParseColor(strxhead(raw,1));
		return true;
	}
	ErrorLogTip(NULL,"数据类型并不是复杂类型 而是"+type,"JSON::GetComplexValue");
//		result = nullptr;
	return false;
}

void pxprint_format(int x, int y, const string& json_list, bool shadowed=false)
{
	if (json_list.length() <= 2)	return;
	vector<string> _list = ParseList(json_list);
	long add_x = 0, add_y = 0;
	long h_max = 0;
	for (size_t i{ 0 }; i < _list.size(); ++i)
	{
		color_t color;
		string text;
		short size;
		short waveColor;	//0 or 1
_beg:
		color = EGERGB(255, 255, 255);
		size = textheight('1');
		waveColor = 0;
		JSON json = ParseJSON(_list.at(i));
		if (json.HaveKey("text"))
			text = json.GetString("text");
		else
			continue;
		if (json.HaveKey("color"))
			color = json.GetColor("color");
		if (json.HaveKey("size"))
			size = json.GetShort("size");
		if (size > h_max)
			h_max = size;
		if (json.HaveKey("waveColor"))
			waveColor = json.GetShort("waveColor");
		bool endLF = false;
		if (strtail(text) == "&")
		{
			text = strxtail(text);
			endLF = true;
		}
		if (text.at(0) != '&' && text.find('&') != string::npos)	//含换行符
		{	//割裂法
			vector<string> texts = CutLine(text, '&');		//split
			json.SetString("text",texts[0]);	//设置本句台词为割裂后的第一部分
			_list.at(i) = json.ToString();
			for (size_t h = 0; h < texts.size() - 1; ++h)
			{
				JSON json_new;
				json_new.AddItem("text", "\"&"+texts.at(h + 1)+"\"");
				json_new.AddItem("color", "#black");		json_new.SetColor("color", color);
				json_new.AddItem("size", "16s");			json_new.SetShort("size", size);
				json_new.AddItem("waveColor", "0s");		json_new.SetShort("waveColor", waveColor);
				auto iter = begin(_list);
				long t = 0;
				while(t < i + h)
					++iter, ++t;
				++iter;
				_list.insert(iter, json_new.ToString());
			}
			goto _beg;
		}
		else if (text.at(0) == '&')
		{
			text = strxhead(text, 1);
			add_x = 0;
			add_y += h_max;
			h_max = 0;
		}
		setfont(size, 0, "Determination Mono");
		if (waveColor)
			setwcolor(color);
		else
			setcolor(color);
		RECT rt = shadowed ? pxprint_shadow(x + add_x, y + add_y, text) : pxprint(x + add_x, y + add_y, text);
		add_x += rt.right - rt.left;
		if (endLF)
		{
			add_x = 0;
			add_y += h_max;
			h_max = 0;
		}
	}
}

#define CT_NORMAL 0
#define CT_BURNING 1
struct CraftTypeData 
{
	char kind;
	short type;	
	CraftTypeData()
	{
		kind = '?';
		type = CT_NORMAL;
	}
};
vector<CraftTypeData> craftTypeData;
#define CRAFTTYPE(ch) (craftTypeData.at(ch-'A'))
#define CRAFTTT(ch) (craftTypeData.at(ch-'A').type)

void ReadCraftTypeData()
{
	if(!ExistFile(CRAFTTYPEDATA_PATH)) {
		ErrorLogTip(NULL,"合成类型数据文件丢失！","ReadCraftTypeData");
		return;
	}
	DebugLog("【读取】读取方块数据："+CRAFTTYPEDATA_PATH);
	vector<string> lines = ReadFileLines(CRAFTTYPEDATA_PATH);
	long i=1;
	while(i < lines.size()) {
		string line = lines.at(i);
		if(line.empty()) {
			++i;
			continue;
		}
		stringstream ss;
		CraftTypeData ctd;
		ss<<line;
		ss>>ctd.kind;
		ss>>ctd.type;
		
		craftTypeData.push_back(ctd);
		++i;
	}
}

#define DEF_BLOCK_CN_NAME "方块"
#define DEF_BLOCK_EN_NAME "BLOCK"

#define BCLASS_INVALID 0
#define BCLASS_AIR 1		//气体 
#define BCLASS_LIQUID 2		//液体 
#define BCLASS_NATURE 3		//自然 
#define BCLASS_PASSABLE 4	//可穿过 
#define BCLASS_TREE 5		//树类 
#define BCLASS_FRUITFUL 6	//可采摘 
#define BCLASS_BLOCK 7
#define BCLASS_CRAFT 8		//合成平台 
#define BCLASS_CONTAINER 9	//容器 
#define BCLASS_DOOR 10		//门类 
#define BCLASS_PICKUP 11	//可以直接捡起，例如火把
#define BCLASS_GRAVESTONE 12//墓碑 
#define BCLASS_BURNER 13	//烧火器具 
#define BCLASS_BLOSSOM 14	//可开花 
#define BCLASS_REPLACEABLE 15	//可替换方块
typedef float LIGHTNESS;	//自己发光亮度，小数 
using SND_FAMILY = USHORT;
using SND_FAMILY_WHEN = USHORT;
#define SNDF_NONE 0
#define SNDF_GRASS 1
#define SNDF_GRAVEL 2
#define SNDF_WOOD 3
#define SNDF_STONE 4
#define SNDF_CLOTH 5
#define SNDF_SAND 6
#define SNDF_SNOW 7
#define SNDF_WATER 8
#define SNDF_LAVA 9
#define SNDF_ANVIL 10
#define SNDF_BUSH 11
#define SNDF_SAPLING 12
#define SNDF_GLASS 13

#define SNDFW_DAMAGE 0
#define SNDFW_PLACE 1
#define SNDFW_BROKEN 2
SND_FAMILY ParseSoundFamily(const string& src)
{
	if (sequ(src, "none"))	return SNDF_NONE;
	if (sequ(src, "grass"))	return SNDF_GRASS;
	if (sequ(src, "gravel"))	return SNDF_GRAVEL;
	if (sequ(src, "wood"))	return SNDF_WOOD;
	if (sequ(src, "stone"))	return SNDF_STONE;
	if (sequ(src, "cloth"))	return SNDF_CLOTH;
	if (sequ(src, "sand"))	return SNDF_SAND;
	if (sequ(src, "snow"))	return SNDF_SNOW;
	if (sequ(src, "water"))	return SNDF_WATER;
	if (sequ(src, "lava"))	return SNDF_LAVA;
	if (sequ(src, "bush"))	return SNDF_BUSH;
	if (sequ(src, "sapling"))	return SNDF_SAPLING;
	if (sequ(src, "anvil"))	return SNDF_ANVIL;
	if (sequ(src, "glass"))	return SNDF_GLASS;
	ErrorLogTip(NULL, "无效的音效家族=" + src, "ParseSoundFamily");
	return SNDF_STONE;
}
void PlayFamilySound(SND_FAMILY sf, SND_FAMILY_WHEN when)
{
	if (sf == SNDF_NONE)	return;
	int rcnt = 4;
	string name = "1";
	if (sf == SNDF_GRASS)	name = "grass";
	if (sf == SNDF_GRAVEL)	name = "gravel";
	if (sf == SNDF_WOOD)	name = "wood";
	if (sf == SNDF_CLOTH)	name = "cloth";
	if (sf == SNDF_SAND)	name = "sand";
	if (sf == SNDF_STONE || sf == SNDF_GLASS)	name = "stone";
	if (sf == SNDF_SNOW)	name = "snow";
	if (sf == SNDF_BUSH)	name = "bush";
	if (sf == SNDF_SAPLING)	name = "sapling", rcnt = 6;
	if (sf == SNDF_WATER)	name = "water", rcnt = 3;
	if (sf == SNDF_LAVA)	name = "lava", rcnt = 3;
	if (sf == SNDF_ANVIL)	name = "anvil", rcnt = 1;
	//上述为默认值（SNDFW_PLACE）
	if (when == SNDFW_DAMAGE)
	{
		if (sf == SNDF_SAPLING || sf == SNDF_BUSH)
			name = "bush", rcnt = 4;
		else if (sf == SNDF_GRASS)
			name = "gravel", rcnt = 4;
		else if (sf == SNDF_ANVIL)
			name = "stone" , rcnt = 4;
	}
	else if (when == SNDFW_BROKEN)
	{
		if (sf == SNDF_ANVIL)
			name = "stone", rcnt = 4;
		else if (sf == SNDF_GLASS)
			name = "glass", rcnt = 3;
	}
	
	if (name == "1")
	{
		ErrorLogTip(NULL, "无效的音效家族=" + ToString(sf), "PlayFamilySound");
		return;
	}
	string packed = ("block\\" + name + ToString(RandomRange(1, rcnt, true, true)));
	SND(packed);
}

class BlockData {
	public:
		BLOCK_ID id;
		string cn_name;
		string en_name;
		color_t txt_color;
		short dur;
		short dur_lvl;
		SND_FAMILY snd_family;

		vector<string> shapes;
		vector<color_t> colors;
		vector<BLOCK_CLASS_ID> classes;

		JSON data;	//JSON

		BlockData() {
			id=0;
			cn_name=DEF_BLOCK_CN_NAME;
			en_name=DEF_BLOCK_EN_NAME;
			txt_color = LIGHTGRAY;
			dur=1;
			dur_lvl=1;
		}
		void ClearShapes() {
			shapes.clear();
			colors.clear();
			classes.clear();
		}
		inline bool IsKeeperContainer(void)
		{
			if(!HaveClass(BCLASS_CONTAINER) || !data.HaveKey("IsKeeper"))	return false;
			short ik=0;
			bool suc = data.GetSimpleValue<short>("IsKeeper",&ik);
			if(!suc)
			{
				ErrorLogTip(NULL,"无法获取方块是否是保持型容器","BlockData::IsKeeperContainer");
				return false;
			}
			return bool(ik);
		}
		inline bool IsLightSource(void) const
		{	//BlockData::
			return (data.HaveKey("Lightness") || HaveClass(BCLASS_BURNER));
		}
		bool HaveClass(BLOCK_CLASS_ID bcid) const{
			//是否属于指定分类
			if(classes.empty())	return false;
			for(short j = 0; j < classes.size(); ++j)
				if(classes.at(j) == bcid)
					return true;
			return false;
		}
		LIGHTNESS GetOwnLightness(void)	const
		{
			LIGHTNESS ln=0;
			bool suc = data.GetSimpleValue<float>("Lightness",&ln);
			if(!suc) {
				ErrorLogTip(NULL,"无法获取方块自身亮度\nid="+ToString(id)+" en_name="+en_name,"BlockData::GetOwnLightness");
				return 0;
			}
			return ln;
		}
		short GetContainerWidth(void) const
		{
			short w=0;
			bool suc = data.GetSimpleValue<short>("CWidth",&w);
			if(!suc) {
				ErrorLogTip(NULL,"无法获取容器方块宽\nid="+ToString(id)+" en_name="+en_name,"BlockData::GetContainerWidth");
				return 0;
			}
			return w;
		}
		short GetContainerHeight(void) const
		{
			short h=0;
			bool suc = data.GetSimpleValue<short>("CHeight",&h);
			if(!suc) {
				ErrorLogTip(NULL,"无法获取容器方块高\nid="+ToString(id)+" en_name="+en_name,"BlockData::GetContainerHeight");
				return 0;
			}
			return h;
		}
		color_t GetContainerBgColor(void) const
		{
			color_t clr=BLACK;
			bool suc = data.GetComplexValue("CBgColor",&clr);
			if(!suc) {
				ErrorLogTip(NULL,"无法获取容器背景色\nid="+ToString(id)+" en_name="+en_name,"BlockData::GetContainerBgColor");
				return 0;
			}
			return clr;
		}
};
vector<BlockData> blockdata;

BLOCK_ID CnNameToBlock(const string& cn_name) {
	for(short i = 0; i < blockdata.size(); ++i)
		if(blockdata.at(i).cn_name == cn_name)
			return BLOCK_ID(i);
	ErrorLogTip(NULL,"无法根据中文ID获取方块ID\ncn_name="+cn_name,"CnNameToBlock");
	return 0;
}
BLOCK_ID EnNameToBlock(const string& en_name) {
	for(short i = 0; i < blockdata.size(); ++i)
		if(blockdata.at(i).en_name == en_name)
			return BLOCK_ID(i);
	ErrorLogTip(NULL,"无法根据英文ID获取方块ID\nen_name="+en_name,"EnNameToBlock");
	return 0;
}

const vector<pair<string, color_t>> g_colorPrefabs {
	{"red",EGERGB(255,0,0)}, {"green",EGERGB(0,255,0) }, { "blue",EGERGB(0,0,255)},
	{"white",EGERGB(255,255,255)}, {"black",EGERGB(0,0,0)}, {"light_gray",LIGHTGRAY}, {"dark_gray",DARKGRAY},
	{"yellow",EGERGB(255,255,0)},  {"magenta",EGERGB(255,0,255)}, {"gold",EGERGB(255,215,0)},
	{"pink", PINK}, {"purple",PURPLE},{"orange",ORANGE}, {"cyan", CYAN}, {"light_cyan", EGERGB(0,255,255)} ,
	{"light_red", LIGHTRED}, {"light_blue", LIGHTBLUE}, {"light_green", LIGHTGREEN},
 };

color_t ParseColor(const string& s) {
	//解析颜色
	if(s.empty()) {
		ErrorLogTip(NULL,"解析颜色字符串遇到错误：空串","ParseColor");
		return WHITE;
	}
	if(s[0] == '&') {	//16色			e.g.   #&F 表示纯白色
		UINT uClr = Convert<string,UINT>(strxhead(s,1));
		return Color16ToRGB(uClr);
	}
	for (size_t j = 0; j < g_colorPrefabs.size(); ++j)
		if (sequ(s, g_colorPrefabs.at(j).first))
			return g_colorPrefabs.at(j).second;
	vector<string> rgb = CutLine(s,';');
	if(rgb.size() < 3) {
		ErrorLogTip(NULL,"解析颜色字符串遇到错误：RGB必须含3个项","ParseColor");
		return WHITE;
	}
	UINT r = Convert<string,UINT>(rgb.at(0));
	UINT g = Convert<string,UINT>(rgb.at(1));
	UINT b = Convert<string,UINT>(rgb.at(2));
	return EGERGB(r,g,b);
}

#define BLOCK_PX_MIN 38
#define BLOCK_PX_MAX 65
#define DEF_BLOCK_INIT_PX 45
#define DEF_BLOCK_PX Clamp<int>(45 * (uiy / 1080.0), BLOCK_PX_MIN, BLOCK_PX_MAX)
int g_block_px = DEF_BLOCK_INIT_PX;

int scene_left, scene_right, scene_top, scene_bottom;
/*#define PL_VIEW_W 16
#define PL_VIEW_H 15*/
#define PL_VIEW_W_HALF ((scene_right - scene_left - 50 ) / g_block_px / 2)
#define PL_VIEW_H_HALF ((scene_bottom - scene_top - 200 ) / g_block_px / 2)

#define CHATITEM_SHOW_TIME 16000
#define CHATITEM_FADE_TIME 500
#define CHATITEM_MAX_CNT 120
#define CHATITEM_MAXLEN 1024
#define CHAT_LEFT int(100*(uix/1920.0))
#define CHAT_BOTTOM int(uiy*0.85)
#define CHAT_WIDTH int(PL_VIEW_W_HALF*2.0*g_block_px)
#define CHAT_FS int(35*(uiy / 1080.0))
#define CHATITEM_SHOW_CNT	   int(PL_VIEW_H_HALF*g_block_px / CHAT_FS + 1)
#define CHATITEM_FULL_SHOW_CNT int(PL_VIEW_H_HALF*2*g_block_px / CHAT_FS + 1)
struct ChatItem
{
	string format;
	clock_t start;

	ChatItem()
	{
		start = clock();
	}
	void Reset(const string& _format)
	{
		format = _format;
		start = clock();
	}
	void ResetPlain(const string& _plaintext)
	{
		format = "[{text:\"" + _plaintext + "\",color:#white}]";
		start = clock();
	}
	inline bool IsActive(void) const
	{
		return (clock() - start <= CHATITEM_SHOW_TIME);
	}
	float GetShowRatio(bool chatOpen) const
	{
		if (chatOpen)	return 1.0f;
		if (clock() - start < CHATITEM_SHOW_TIME - CHATITEM_FADE_TIME)
			return 1.0f;
		if (clock() - start > CHATITEM_SHOW_TIME)
			return 0.0f;
		return 1.0f - (clock() - start - CHATITEM_SHOW_TIME + CHATITEM_FADE_TIME) / (float)CHATITEM_FADE_TIME;
	}
};
vector<ChatItem> g_chat;
void UpdateChatItems()
{
	while (g_chat.size() > CHATITEM_MAX_CNT)
	{
		g_chat.erase(g_chat.begin());
	}
}
void AddChatItem(const string& format)
{
	ChatItem ci;
	ci.Reset(format);
	g_chat.push_back(ci);
}
void AddChatItemPlain(const string& plaintext)
{
	ChatItem ci;
	ci.ResetPlain(plaintext);
	g_chat.push_back(ci);
}
void ClearChatItems()
{
	g_chat.clear();
}
class ChatBox
{
private:
	sys_edit* pchat_box = nullptr;

public:
	ChatBox(){}
	~ChatBox()
	{
		if (pchat_box)
			Dispose();
	}
	inline bool Dead() const
	{
		return pchat_box == nullptr;
	}
	void Init()
	{
		if (pchat_box)
			delete pchat_box , pchat_box = nullptr;
		pchat_box = new sys_edit();
		pchat_box->create();
		pchat_box->setbgcolor(BLACK);
		pchat_box->setcolor(WHITE);
		pchat_box->setfont(CHAT_FS, 0, "Determination Mono");
		pchat_box->setmaxlen(CHATITEM_MAXLEN);
		pchat_box->settext("");
		pchat_box->move(CHAT_LEFT - 1, CHAT_BOTTOM+2);
		pchat_box->size(CHAT_WIDTH, CHAT_FS + 3);
		pchat_box->enable(false);
	}
	inline void Enable()
	{
		if (!pchat_box)	return;
		pchat_box->enable(true);
	}
	inline void Disable()
	{
		if (!pchat_box)	return;
		pchat_box->enable(false);
	}
	inline void Show()
	{
		if (!pchat_box)	return;
		pchat_box->visable(true);
		pchat_box->setfocus();
	}
	inline void Hide()
	{
		if (!pchat_box)	return;
		pchat_box->visable(false);
	}
	inline void Dispose()
	{
		if (pchat_box)
		{
			pchat_box->enable(false);
			pchat_box->destory();
			delete pchat_box;
			pchat_box = nullptr;
		}
	}
	inline bool IsFocused(void) const
	{
		if (!pchat_box)	return false;
		return pchat_box->isfocus();
	}
	inline void SetFocus(void)
	{
		if (!pchat_box)	return;
		pchat_box->setfocus();
	}
	string GetText(void) const
	{
		if (!pchat_box) return "";
		char buf[CHATITEM_MAXLEN]{ 0 };
		pchat_box->gettext(CHATITEM_MAXLEN, buf);
		return buf;
	}
	void ClearText(void)
	{
		if (!pchat_box)	return;
		pchat_box->settext("");
	}
};

BLOCK_CLASS_ID ParseBlockClass(const string& s) {
	if (sequ(s, "air"))
		return BCLASS_AIR;
	else if (sequ(s, "liquid"))
		return BCLASS_LIQUID;
	else if (sequ(s, "nature"))
		return BCLASS_NATURE;
	else if (sequ(s, "passable"))
		return BCLASS_PASSABLE;
	else if (sequ(s, "tree"))
		return BCLASS_TREE;
	else if (sequ(s, "fruitful"))
		return BCLASS_FRUITFUL;
	else if (sequ(s, "block"))
		return BCLASS_BLOCK;
	else if (sequ(s, "craft"))
		return BCLASS_CRAFT;
	else if (sequ(s, "container"))
		return BCLASS_CONTAINER;
	else if (sequ(s, "door"))
		return BCLASS_DOOR;
	else if (sequ(s, "pickup"))
		return BCLASS_PICKUP;
	else if (sequ(s, "gravestone"))
		return BCLASS_GRAVESTONE;
	else if (sequ(s, "burner"))
		return BCLASS_BURNER;
	else if (sequ(s, "blossom"))
		return BCLASS_BLOSSOM;
	else if (sequ(s, "replaceable"))
		return BCLASS_REPLACEABLE;
	else {
		ErrorLog("无法识别的分类字符串："+s,"ParseBlockClass");
		return BCLASS_INVALID;
	}
}

typedef USHORT	ITEM_TYPE;
#define ITT_NULL 0
#define ITT_BLOCK 1
#define ITT_MATERIAL 2
#define ITT_BOW 3
#define ITT_CROSSBOW 4
#define ITT_GUN 5
#define ITT_RPG 6
#define ITT_LAUNCHER 7
#define ITT_CLOSE_WEAPON 8
#define ITT_PICKAXE 9
#define ITT_AXE 10
#define ITT_ARROW 11
#define ITT_BULLET 12
#define ITT_ROCKET 13
#define ITT_OTHER_PROJECTILE 14
#define ITT_FOOD 15
#define ITT_THROWABLE 16	//便携式投掷物

ITEM_TYPE ParseItemType(const string& s) {
	if (sequ(s, "null"))
		return ITT_NULL;
	else if (sequ(s, "material"))
		return ITT_MATERIAL;
	else if (sequ(s, "block"))
		return ITT_BLOCK;
	else if (sequ(s, "pickaxe"))
		return ITT_PICKAXE;
	else if (sequ(s, "axe"))
		return ITT_AXE;
	else if (sequ(s, "bow"))
		return ITT_BOW;
	else if (sequ(s, "close_weapon"))
		return ITT_CLOSE_WEAPON;
	else if (sequ(s, "crossbow"))
		return ITT_CROSSBOW;
	else if (sequ(s, "gun"))
		return ITT_GUN;
	else if (sequ(s, "food"))
		return ITT_FOOD;
	else if (sequ(s, "rpg"))
		return ITT_RPG;
	else if (sequ(s, "launcher"))
		return ITT_LAUNCHER;	//其他发射器
	else if (sequ(s, "arrow"))
		return ITT_ARROW;
	else if (sequ(s, "bullet"))
		return ITT_BULLET;
	else if (sequ(s, "rocket"))
		return ITT_ROCKET;
	else if (sequ(s, "throwable"))
		return ITT_THROWABLE;
	else if(sequ(s,"other_projetile"))
		return ITT_OTHER_PROJECTILE;
	else
		return ITT_NULL;
}
vector<Weight<BLOCK_ID>> ParseWBlocks(const string& slist) {
	vector<Weight<BLOCK_ID>> ret;
	vector<string> sItems = CutLine(slist,';');
	for(short i = 0; i < sItems.size(); ++i) {
		Weight<BLOCK_ID> wb;
		vector<string> sSubItem = CutLine(sItems.at(i),'*');
		if(sSubItem.size() < 2) {
			ErrorLogTip(NULL,"解析加权方块列表时发现错误：\n单加权方块项目不符合格式（英文方块ID*权重）","ParseWBlocks");
			continue;
		}
		wb.value = EnNameToBlock(sSubItem[0]);
		wb.weight = Convert<string,short>(sSubItem[1]);
		ret.push_back(wb);
	}
	return ret;
}
class Tip {
	public:
		string text;
		Tip() {
		}
};
vector<Tip> tips;
void ReadTips() {
	if(!ExistFile(TIPS_PATH)) {
		DebugLog("没有发现游戏提示数据！");
		return;
	}
	DebugLog("【读取】读取游戏提示："+TIPS_PATH);
	vector<string> lines = ReadFileLines(TIPS_PATH);
	for(short i = 0; i < lines.size(); ++i) {
		Tip tip;
		tip.text = lines.at(i);
		tips.push_back(tip);
	}
}

void ReadBlockData() {
	if(!ExistFile(BLOCKDATA_PATH)) {
		ErrorLogTip(NULL,"方块数据文件丢失！","ReadBlockData");
		return;
	}
	DebugLog("【读取】读取方块数据："+BLOCKDATA_PATH);
	vector<string> lines = ReadFileLines(BLOCKDATA_PATH);
	long i=1;
	while(i < lines.size()) {
		string line = lines.at(i);
		if(line.empty()) {
			++i;
			continue;
		}
		line = CSVResplitLine(line);
//		DebugLog(line);

		stringstream ss;
		BlockData bdt;
		ss<<line;

		ss>>bdt.id;
		ss>>bdt.cn_name;

		string tmp;
		ss>>tmp;
//		DebugLog(tmp);
		bdt.txt_color = ParseColor(tmp);
		ss>>bdt.en_name;
		ss>>bdt.dur;
		ss>>bdt.dur_lvl;

		ss>>tmp;
		bdt.shapes = CutLine(tmp,';');

		ss>>tmp;
		vector<string> sColors = CutLine(tmp,'+');
		for(short i = 0; i < sColors.size(); ++i)
			bdt.colors.push_back(ParseColor(sColors[i]));

		ss>>tmp;
		vector<string> sClasses = CutLine(tmp,';');
		for(short i = 0; i < sClasses.size(); ++i)
			bdt.classes.push_back(ParseBlockClass(sClasses[i]));
		ss >> tmp;
		bdt.snd_family = ParseSoundFamily(tmp);

		string tmptmp="";
		short c=0;
		while(!ss.eof()) {
			ss>>tmp;
			if(c!=0)	//2,3,4etc
				tmptmp+=",";
			tmptmp+=tmp;
			c++;
		}
		if(strhead(tmptmp)=="\"")
			tmptmp = strrpc((char*)strxhead(strxtail(tmptmp)).c_str(),"\"\"","\"");
		bdt.data = ParseJSON(tmptmp,"ReadBlockData");

		blockdata.push_back(bdt);
		++i;
	}
}
void LoadOtherImages();
bool firstRun=false;
void Splash() {
	TrackLog("...");
	setcolor(WHITE);
	setfont(95*(uiy/1080.0),0,"FixedSys");
	midprinty("Wormwake",uiy*0.3f);
	setcolor(LIGHTGRAY);
	setfont(70*(uiy/1080.0),0,"FixedSys");
	midprinty("G A M E S ",uiy*0.3f+100*(uiy/1080.0));
	setfont(33*(uiy/1080.0),0,"Consolas");
	setcolor(YELLOW);
	midprinty("LOADING...",uiy*0.3f+180 * (uiy / 1080.0));
	delay_fps(40);
	setfont(31*(uiy/1080.0),0,"宋体");
	setcolor(LIGHTGRAY);
	setbkmode(OPAQUE);
	const short sz = uiy - 120 * (uiy / 1080.0);
	midprinty("读取配置文件...",sz);
	delay_fps(30);
	ReadOptions();
	midprinty("读取方块数据BlockData...",sz);
	delay_fps(30);
	ReadBlockData();
	midprinty("读取物品数据ItemData...",sz);
	delay_fps(30);
	ReadItemData();
	midprinty("读取生物群系数据BiomeData...",sz);
	delay_fps(30);
	ReadBiomeData();
	midprinty("  读取实体数据EntityData...  ",sz);
	delay_fps(30);
	ReadEntityData();
	midprinty("  读取效果数据EffectData...  ", sz);
	delay_fps(30);
	ReadEffectData();
	midprinty("  读取成就数据AchievementData...  ",sz);
	delay_fps(30);
	ReadAchievementData();
	midprinty("  读取合成类型数据CraftTypeData... ",sz);
	delay_fps(30);
	ReadCraftTypeData();
	midprinty("读取伤害位点数据DamagePointData...", sz);
	delay_fps(30);
	ReadDPData();
	midprinty("读取方块默认JSON数据BlockDefaultJSONData...",sz);
	delay_fps(30);
	ReadBlockDefaultJSONData();
	midprinty(" 读取物品默认JSON数据ItemDefaultJSONData... ",sz);
	delay_fps(30);
	ReadItemDefaultJSONData();
	midprinty("             读取配方Recipes...             ",sz);
	delay_fps(30);
	ReadRecipes();
	midprinty("               读取其他内容...             ", sz);
	delay_fps(30);
	ReadTips();
	LoadOtherImages();

	randomize();
	midprinty("                 准备音频...               ", sz);
	delay_fps(30);
}
void sgap(long ms) {
	clock_t start=clock();
	while(clock() - start < ms)
		if(kbmsg() || K(VK_LBUTTON))
			break;
}
void Beginning() {
	//开端剧情
	static int beg_text_fs = 55 * (uiy / 1080.0);
	DebugLog("进入故事的开端");
	cls();
//	delay_ms(0);
	delay(1000);
	setfont(beg_text_fs,0,"等线");
	setcolor(YELLOW);
	midprinty("故事还得从头说起...",uiy*0.55,0,uix,100);
	sgap(1000);
	cls();

	setcolor(PINK);
	midprinty("这本是一个美不胜收的世界.",uiy*0.45,0,uix,150);
	sgap(1500);
	setcolor(LIGHTGREEN);
	midprinty("花儿在绽放，鸟儿在歌唱...",uiy*0.45+beg_text_fs+10,0,uix,80);
	sgap(900);
	setcolor(CORNSILK);
	midprinty("无数人类沉浸在美妙的大自然中，过着美满的生活。",uiy*0.45+beg_text_fs*2+20,0,uix,70);
	sgap(4000);
	cls();

	setcolor(WHITE);
	midprinty("但随着科技的飞速发展，",uiy*0.38,0,uix,180);
	sgap(900);
	setcolor(LIGHTGRAY);
	midprinty("人类对各式新奇的电子产品痴迷过度",uiy*0.38+beg_text_fs+40,0,uix,120);
	sgap(1200);
	setcolor(DARKGRAY);
	midprinty("逐渐丧失了对美的鉴赏能力...",uiy*0.38+beg_text_fs*2+80,0,uix,150);
	sgap(2400);
	cls();

	const int _etop = uiy * 0.2 - beg_text_fs - 40;
	midprinty("旅游业从此迅速地衰微...",_etop+beg_text_fs+40,0,uix,220);
	sgap(1200);
	setcolor(WHITE);
	midprinty("但电子竞技,元宇宙等高端技术愈加发达..",_etop+beg_text_fs*2+40*2,0,uix,240);
	sgap(2600);
	cls();
	setcolor(DARKGRAY);
	midpxprint("旅游业从此迅速衰微..",_etop+beg_text_fs+40,0,uix,0);
	setfont(beg_text_fs,0,"等线");
	midprinty("但电子竞技,元宇宙等高端技术越来越发达...",_etop+beg_text_fs*2+40*2,0,uix,0);
	setcolor(WHITE);
	midprinty("自然界逐渐在熟视无睹中渐渐退化.",_etop+beg_text_fs*3+40*3,0,uix,250);
	sgap(3300);
	cls();
	setcolor(DARKGRAY);
	midpxprint("旅游业迅速衰微.",_etop+beg_text_fs+40,0,uix,0);
	midpxprint("然而电子竞技,元宇宙等高端科学技术越来越发达....",_etop+beg_text_fs*2+40*2,0,uix,0);
	setfont(beg_text_fs,0,"等线");
	midprinty("自然界逐渐在熟视无睹中渐渐退化..",_etop+beg_text_fs*3+40*3,0,uix,0);
	setcolor(WHITE);
	midprinty("情况逐渐失控，",_etop+beg_text_fs*4+40*4,0,uix,300);
	sgap(3300);
	cls();
	setcolor(DARKGRAY);
	midpxprint("旅游业衰微.",_etop+beg_text_fs+40,0,uix,0);
	midpxprint("然而电子竞技,元宇宙等高端科学技术越来越发达.....",_etop+beg_text_fs*2+40*2,0,uix,0);
	midpxprint("自然界逐渐在熟视无睹中渐渐退化...",_etop+beg_text_fs*3+40*3,0,uix,0);
	midpxprint("情况逐渐失控..",_etop+beg_text_fs*4+40*4,0,uix,0);
	setcolor(CORNSILK);
	setfont(beg_text_fs,0,"等线");
	midprinty("整个世界转变成了一个充满符号的抽象世界",_etop+beg_text_fs*5+40*5,0,uix,200);
	sgap(1500);
	cls();
	setcolor(DARKGRAY);
	midpxprint("然而电子竞技,元宇宙等高端科学技术越来越发达.....",_etop+beg_text_fs*2+40*2,0,uix,0);
	midpxprint("自然界逐渐在熟视无睹中渐渐退化...",_etop+beg_text_fs*3+40*3,0,uix,0);
	midpxprint("情况逐渐失控...",_etop+beg_text_fs*4+40*4,0,uix,0);
	setcolor(YELLOW);
	midpxprint("整个世界转变成了一个充满符号的抽象世界",_etop+beg_text_fs*5+40*5,0,uix,0);
	setcolor(WHITE);
	midprinty("是谓『暗黑维度』.",_etop+beg_text_fs*6+40*6,0,uix,500);
	setcolor(LIGHTGRAY);
	setfont(beg_text_fs,0,"Calibri");
	midprinty("   DarkVoxel   ",_etop+beg_text_fs*7+40*7+5,0,uix,50);
	sgap(1000);
	cls();
	setcolor(YELLOW);
	midpxprint("整个世界转变成了一个充满符号的抽象世界",_etop+beg_text_fs*5+40*5,0,uix,0);
	setcolor(WHITE);
	midpxprint("是谓『暗黑维度』.",_etop+beg_text_fs*6+40*6,0,uix,0);
	setcolor(LIGHTGRAY);
	midpxprint("       DarkVoxel   ",_etop+beg_text_fs*7+40*7+5,0,uix,5);
	sgap(4000);

	setcolor(DARKGRAY);
	midpxprint("[按任意键开始游戏]",uiy*0.88,0,uix,50);
	delay(100);
	getch();
	cls();
}

#define DRAW_CD 0
namespace ui_data {
	RECT rt_splay;
	RECT rt_opt;
	RECT rt_leave;
	short ui_main_pre_id = -1;

	int ui_subtitle_fs = 65;
#define OPTION_COL_CNT 5
	RECT rt_opt_esc;
	RECT rt_opt_save_opendir;
	RECT rt_opt_save_delete;
	RECT rt_opt_mus_switch;
	RECT rt_opt_snd_switch;
	const array<string, OPTION_COL_CNT> ui_opt_cols = {"存档", "控制", "规则", "视频", "音频"};
#define OPTION_RULE_ITEM_CNT 4
	array<RECT, OPTION_RULE_ITEM_CNT> rt_opt_rules;
	const array<string, OPTION_RULE_ITEM_CNT> ui_opt_rules_texts
		= { "自言自语" , "生成怪物", "死亡掉落" , "生物掉落"};
	array<bool*, OPTION_RULE_ITEM_CNT> ui_opt_rules_switches
		= { &options.player_mutter , &options.mob_spawning, &options.death_drop, &options.creature_drop};
	short ui_opt_rules_pre_id = -1;
#define OPTION_VIDEO_ITEM_CNT 6
#define OPV_SHOW_BTAG 0
#define OPV_SHOW_ETAG 1
#define OPV_SHOW_ITAG 2
#define OPV_SHOW_WEATHER 3
#define OPV_USE_DUR_BAR 4
#define OPV_VIEW_SIDE_FX 5
	array<RECT, OPTION_VIDEO_ITEM_CNT> rt_opt_videos;
	const array<string, OPTION_VIDEO_ITEM_CNT> ui_opt_videos_texts
		= {"显示方块标签", "显示实体标签", "显示物品标签", "显示天气效果" , "使用耐久条", "视野边界效果"};
	array<bool*, OPTION_VIDEO_ITEM_CNT> ui_opt_videos_switches
		= {&options.blocktag_switch, &options.entitytag_switch, &options.itemtag_switch,
			&options.show_weather, &options.durability_bar, &options.view_side_fx};
	short ui_opt_videos_pre_id = -1;
	array<RECT, OPTION_COL_CNT> rt_opt_cols;
	short ui_opt_col_id=0;
	short ui_opt_col_pre_id = -1;	//except video settings

	RECT rt_play_create_new;
	RECT rt_play_giveup;
	short ui_play_cc_pre_id = -1;

	RECT rt_nvcfg_done;
	RECT rt_nvcfg_cancel;
	int ui_nvcfg_title_fs = 65;
	int ui_nvcfg_text_fs = 55;
	int ui_nvcfg_btn_fs = 60;
	short ui_nvcfg_pre_id = -1;

	short craft_precur = -1;	//相对值 从上到下，0开始
	string block_op_tip="";		//对方块操作的提示
	bool bPaused = false;		//是否暂停 
	
	RECT rt_pm_back;
	RECT rt_pm_achv;
	RECT rt_pm_opt;
	RECT rt_pm_save;
	RECT rt_pm_sq;
	short ui_pm_pre_id = -1;
	
	short acd_top_cur=1;
	PIMAGE white_screen;
	PIMAGE black_screen;

	void InitSizes()
	{
		ui_subtitle_fs *= (uiy / 1080.0);
		ui_nvcfg_title_fs *= (uiy / 1080.0);
		ui_nvcfg_text_fs *= (uiy / 1080.0);
		ui_nvcfg_btn_fs *= (uiy / 1080.0);
	}
}
#define uid ::ui_data::

void LoadOtherImages()
{
	uid white_screen = newimage(uix, uiy);
	uid black_screen = newimage(uix, uiy);
	for(int i = 0; i < uiy; ++i)
		for (int j = 0; j < uix; ++j)
		{
			putpixel(j, i, EGERGB(255, 255, 255), uid white_screen);
			putpixel(j, i, EGERGB(0, 0, 0), uid black_screen);
		}
}

using KBLEVEL = USHORT;	//Knockback Level

typedef USHORT AI_TYPE, ENTITY_TYPE;
//实体类型
#define ETT_NULL 0
#define ETT_ARROW 1
#define ETT_BULLET 2
#define ETT_ROCKET 3
#define ETT_OTHER_PROJECTILE 4
#define ETT_MONSTER 5
#define ETT_ITEM 6
#define ETT_ANIMAL 7
#define ETT_BLADE 8
inline bool IsProjectileET(ENTITY_TYPE ett)
{
	return (ett == ETT_ARROW || ett == ETT_BULLET || ett == ETT_ROCKET || ett == ETT_OTHER_PROJECTILE);
}
inline bool IsCreature(ENTITY_ID eid)
{
	return (eid == ETT_ANIMAL || eid == ETT_MONSTER);
}
ENTITY_TYPE ParseEntityType(const string& src) {
	if(sequ(src,"item"))
		return ETT_ITEM;
	if(sequ(src,"null"))
		return ETT_NULL;
	if (sequ(src, "blade"))
		return ETT_BLADE;	
	if(sequ(src,"arrow"))
		return ETT_ARROW;
	if(sequ(src,"bullet"))
		return ETT_BULLET;
	if(sequ(src,"rocket"))
		return ETT_ROCKET;
	if(sequ(src,"other_projectile"))
		return ETT_OTHER_PROJECTILE;
	if(sequ(src,"monster"))
		return ETT_MONSTER;
	if (sequ(src, "animal"))
		return ETT_ANIMAL;
	ErrorLogTip(NULL,"无效的实体类型:"+src,"ParseEntityType");
	return ETT_NULL;
}
//AI类型
#define AI_STILL 0
#define AI_CHASE 1
#define AI_LINE 2
#define AI_FLEE 3
#define AI_IDLE 4
//Entity Attacking Mode
#define EAM_CLOSE 0
#define EAM_FAR 1

class EntityData {	//实体数据
	public:
		ENTITY_ID id;
		string cn_name;
		string en_name;
		//实体类型
		ENTITY_TYPE type;
		//形态&颜色
		vector<string> shapes;
		vector<color_t> colors;
		//JSON数据 包括最大血量等
		JSON data;

		EntityData() {
			id = 0;
			cn_name = "未知实体";
			en_name = "UnknownEntity";
			type = ETT_NULL;
		}
		short GetMaxHp(void) const {
			short ret=0;
			bool suc = data.GetSimpleValue<short>("MaxHp",&ret);
			if(!suc) {
				ErrorLogTip(NULL,"无法获取实体最大血量\nid="+ToString(ret),"EntityData::GetMaxHp");
				return 0;
			}
			return ret;
		}
		clock_t GetAttackCD(void) const {
			if (!data.HaveKey("AttackCD"))
				return LONG_MAX;
			short ret=0;
			bool suc = data.GetSimpleValue<short>("AttackCD",&ret);
			if(!suc) {
				ErrorLogTip(NULL,"无法获取实体攻击冷却\nid="+ToString(ret),"EntityData::GetAttackCD");
				return 0;
			}
			return ret;
		}
		AI_TYPE GetAIType(void) const {
			short ret=0;
			if(!data.HaveKey("AIType"))
				return AI_STILL;

			bool suc = data.GetSimpleValue<short>("AIType",&ret);
			if(!suc) {
				ErrorLogTip(NULL,"无法获取实体默认AI类型\nid="+ToString(ret),"EntityData::GetAIType");
				return 0;
			}
			return ret;
		}
		short GetExtraDamage(void) const
		{
			if(!data.HaveKey("EDamage"))
				return 0;
			short ed=0;
			bool suc = data.GetSimpleValue<short>("EDamage",&ed);
			if(!suc) {
				ErrorLogTip(NULL,"无法获取实体额外伤害\nid="+ToString(id),"EntityData::GetExtraDamage");
				return 0;
			}
			return ed;
		}
		short GetDPID(void) const
		{
			if (!data.HaveKey("DamagePointsID"))
				return 0;
			short dpid = 0;
			bool suc = data.GetSimpleValue<short>("DamagePointsID", &dpid);
			if (!suc) {
				ErrorLogTip(NULL, "无法获取伤害位点ID\neid=" + ToString(id), "EntityData::GetDPID");
				return 0;
			}
			return dpid;
		}
		inline bool IsLightSource(void) const
		{	//EntityData::
			return data.HaveKey("Lightness");
		}
		LIGHTNESS GetOwnLightness(void) const
		{
			LIGHTNESS ln=0.0f;
			bool suc = data.GetSimpleValue<float>("Lightness",&ln);
			if(!suc) {
				ErrorLogTip(NULL,"无法获取实体自身亮度\nid="+ToString(id)+" en_name="+en_name,"EntityData::GetOwnLightness");
				return 0;
			}
			return ln;
		}
		vector<short> GetAttackModes(void) const
		{
			vector<short> ret;
			if(!data.HaveKey("AttackModes"))
			{	//默认只有近战 
				ret.push_back(EAM_CLOSE);
				return ret;
			}
			vector<string> sam;
			bool suc = data.GetComplexValue("AttackModes",&sam);
			if(!suc)
			{
				ErrorLogTip(NULL,"无法获取实体攻击方式\nid="+ToString(id),"EntityData::GetAttackModes");
				return ret;
			}
			for(short i = 0; i < sam.size(); ++i)
			{
				ret.push_back(Convert<string,short>(UncoverValue(sam.at(i),"short")));
			}
			return ret;
		}
		bool CanCloseAttack(void) const
		{
			vector<short> attmodes = GetAttackModes();
			if(attmodes.empty())	return false;
			for(short j = 0; j < attmodes.size(); ++j)
				if(attmodes.at(j) == EAM_CLOSE)
					return true;
			return false;
		}
		bool CanFarAttack(void) const
		{
			vector<short> attmodes = GetAttackModes();
			if(attmodes.empty())	return false;
			for(short j = 0; j < attmodes.size(); ++j)
				if(attmodes.at(j) == EAM_FAR)
					return true;
			return false;
		}
		bool CanSuchAttack(short atm) const
		{
			vector<short> attmodes = GetAttackModes();
			if(attmodes.empty())	return false;
			for(short j = 0; j < attmodes.size(); ++j)
				if(attmodes.at(j) == atm)
					return true;
			return false;
		}
};
vector<EntityData> entitydata;

void ReadEntityData() {
	XCPT_L

	if(!ExistFile(ENTITYDATA_PATH)) {
		ErrorLogTip(NULL,"实体数据丢失","ReadEntityData");
		return;
	}
	DebugLog("【读取】读取实体数据："+ENTITYDATA_PATH);
	vector<string> lines = ReadFileLines(ENTITYDATA_PATH);
	long i=1;
	while(i < lines.size()) {
		string line = lines.at(i);
		if(line.empty()) {
			++i;
			continue;
		}
		line = CSVResplitLine(line);

		stringstream ss;
		EntityData edt;
		ss<<line;

		ss>>edt.id;
		ss>>edt.cn_name;
		ss>>edt.en_name;
		if(edt.id == 0) {
			entitydata.push_back(edt);
			++i;
			continue;
		}

		string tmp="";
		ss>>tmp;
		if(tmp != "null")
			edt.shapes = CutLine(tmp,';');

		ss>>tmp;
		if(tmp != "null") {
			vector<string> sColors = CutLine(tmp,'+');
			for(short i = 0; i < sColors.size(); ++i)
				edt.colors.push_back(ParseColor(sColors[i]));
		}
		ss>>tmp;
		edt.type = ParseEntityType(tmp);

		string tmptmp="";
		short c=0;
		while(!ss.eof()) {
			ss>>tmp;
			if(c!=0)	//2,3,4etc
				tmptmp+=",";
			tmptmp+=tmp;
			c++;
		}
		if(strhead(tmptmp)=="\"")
			tmptmp = strrpc((char*)strxhead(strxtail(tmptmp)).c_str(),"\"\"","\"");
		edt.data = ParseJSON(tmptmp,"ReadEntityData");

		entitydata.push_back(edt);
		++i;
	}

	XCPT_R
}

inline int GetPlayerDrawLeft(void) {
	return scene_left + (scene_right - scene_left)/2 - g_block_px/2;
}
inline int GetPlayerDrawTop(void) {
	return scene_top + (scene_bottom - scene_top)/2 - g_block_px/2;
}

#define DI_LIFE (1000 * 1.5)	//显示时间(ms)
#define DI_ENDING_RATIO 0.2		//结尾占比
#define DI_ANIMATION_SPEED 0.04f	//动画速率
#define DI_X_VARIENCE 0.6f		//左右晃动限度(bk)
#define DI_UP_MAX 2.3f		//动画上升限度
#define DI_FONT "Courier"
#define DI_FS (g_block_px*0.75)
#define DI_LARGE_FS (g_block_px*1.4)	//用于标识死亡 看得舒服

#define DIS_DAMAGE 0x00
#define DIS_HARM DIS_DAMAGE
#define DIS_CRIT   0x01
#define DIS_EFFECT 0x02
#define DIS_HEALTH 0x03
class DamageIndicator {	//伤害指示器
public:
	double x;
	double y;
	UINT num;
	BYTE style;
	clock_t start;
	bool large;

	DamageIndicator()
	{
		x = 0, y = 0;
		num = 1;
		style = DIS_DAMAGE;
		start = clock();
		large = false;
	}
	void Reset(double _x, double _y, int _num, BYTE _style = DIS_DAMAGE, bool _large = false)
	{
		x = _x, y = _y;
		num = _num, style = _style;
		large = _large;
		Start();
	}
	void Start()
	{
		start = clock();
	}
	bool IsAlive(void) const
	{
		return (clock() - start <= DI_LIFE);
	}
	inline color_t GetColor() const
	{
		if (style == DIS_HARM)
			return EGERGB(255, 140, 0);
		else if (style == DIS_CRIT)
			return EGERGB(255, 48, 48);
		else if (style == DIS_HEALTH)
			return EGERGB(144, 238, 144);
		else if (style == DIS_EFFECT)
			return EGERGB(255, 215, 0);
		else {
			return EGERGB(255, 255, 255);
		}
	}
	void VarienceX()
	{
		double r = RandomRangeDouble(-DI_X_VARIENCE, DI_X_VARIENCE, true, true, 2);
		x += r + 0.5;
	}
	double Animate()
	{	//y=f(x)
		double tmp;
		clock_t _x = clock() - start - 700;
		if (_x <= 0)
		{
			tmp = (DI_ANIMATION_SPEED * 0.0001f
				*   -pow2(_x)
				+ 3.0f);
		}
		else {
			tmp = 3.0f;
		}

		ClampA(tmp, (double)0.0f, (double)DI_UP_MAX);
		return tmp;
	}

#define PL_VIEW_RADIUS sqrt(pow2(PL_VIEW_W_HALF)+pow2(PL_VIEW_H_HALF))
#define PL_VIEW_DIAMETER (PL_VIEW_RADIUS*2.0)
#define PL_VIEW_TOP (GetPlayerY()-PL_VIEW_H_HALF)
#define PL_VIEW_BOTTOM (GetPlayerY()+PL_VIEW_H_HALF)
#define PL_VIEW_LEFT (GetPlayerX()-PL_VIEW_W_HALF)
#define PL_VIEW_RIGHT (GetPlayerX()+PL_VIEW_W_HALF)
#define SURROUNDING_RANGE GetPlayerX()-PL_VIEW_W_HALF*2,GetPlayerY()-PL_VIEW_H_HALF*2,GetPlayerX()+PL_VIEW_W_HALF*2,GetPlayerY()+PL_VIEW_H_HALF*2
	void Draw()
	{	//DamageIndicator::Dra w()
		if (x < PL_VIEW_LEFT || x > PL_VIEW_RIGHT || y < PL_VIEW_TOP || y > PL_VIEW_BOTTOM)	//出视野了 
			return;
		color_t clr = GetColor();
		//Strange formula
		double scr_x;
		double scr_y;
		scr_x = GetPlayerDrawLeft() + (x - GetPlayerX()) * g_block_px;
		scr_y = GetPlayerDrawTop() + (y - GetPlayerY() - Animate() + 1.0f) * g_block_px;
		
		double fs = double(large ? DI_LARGE_FS : DI_FS);
		/*if((clock()-start)/(double)DI_LIFE > (1 - DI_ENDING_RATIO))
			fs *= (1.0-(clock()-start)/(double)DI_LIFE)/(double)DI_ENDING_RATIO);	//ËõÐ¡ */
		setfont(fs, 0, DI_FONT);
		setbkmode(TRANSPARENT);
		setcolor(BLACK);
		xyprintf(scr_x - textwidth(ToString(num).c_str()) / 2 + 2, scr_y + 2,
			"%d", num);
		setwcolor(clr);
		xyprintf(scr_x - textwidth(ToString(num).c_str()) / 2, scr_y,	
			"%d", num);
	}
};
vector<DamageIndicator> dmgs;
void CreateDamageIndicator(double x, double y, int num = 1, BYTE style = DIS_HARM, bool large = false)
{	
	DamageIndicator di;
	di.Reset(x, y, num, style, large);
	di.VarienceX();	
	dmgs.push_back(di);
}
void DrawDamageIndicators(void)
{
	if (dmgs.empty())	return;
	for (int i = 0; i < dmgs.size(); ++i)
	{
		dmgs.at(i).Draw();
	}
}
void CheckDamageIndicatorsLife(void)
{
	if (dmgs.empty())	return;
	XCPT_L
	vector<DamageIndicator>::iterator iter;
	//_retry:

	int i = 0;

	for (i = 0; i < dmgs.size(); ++i)
	{
		if (!dmgs.at(i).IsAlive())
		{
			iter = dmgs.begin();
			int j = 0;
			while (iter != dmgs.end() && j != i)
			{
				iter++;
				j++;
			}
			iter = dmgs.erase(iter);
		}
	}

	XCPT_R
}
typedef ULONG ITEM_ID;
string GetItemInitialTag(ITEM_ID id);
void SpawnItem(long x, long y, ITEM_ID id, short cnt, const char* itag=nullptr);

DIR GetChasingDir(long ox, long oy, long px, long py)
{
	if(ox == px)
		return (oy > py ? UP : DOWN);
	if(oy == py)
		return (ox > px ? LEFT : RIGHT);
	if(ox > px)
	{
		if(oy > py)
			return Choice({LEFT,UP});
		return Choice({LEFT,DOWN});
	}else{
		if(oy > py)
			return Choice({RIGHT,UP});
		return Choice({RIGHT,DOWN});
	}
	ErrorLogTip(NULL,"代码出现灵异事件","GetChasingDir");
	return RIGHT;
}
inline DIR GetFleeingDir(long ox, long oy, long px, long py)
{
	return OppoDir(GetChasingDir(ox,oy,px,py));
}
using DPID = USHORT;

using EFFECT_ID = USHORT;
class EffectData {	//效果数据
public:
	EFFECT_ID id;
	string cn_name;
	string en_name;
	string shape;
	color_t color;

	EffectData()
	{
		id = 0;
		cn_name = "无效果";
		en_name = "no_effect";
		shape = "空";
		color = WHITE;
	}
};
vector<EffectData> effectdata;

void ReadEffectData()
{
	XCPT_L

		if (!ExistFile(EFFECTDATA_PATH)) {
			ErrorLogTip(NULL, "效果数据丢失", "ReadEffectData");
			return;
		}
	DebugLog("【读取】读取效果数据：" + EFFECTDATA_PATH);
	vector<string> lines = ReadFileLines(EFFECTDATA_PATH);
	long i = 1;
	while (i < lines.size()) {
		string line = lines.at(i);
		if (line.empty()) {
			++i;
			continue;
		}
		line = CSVResplitLine(line);

		stringstream ss;
		EffectData edt;
		ss << line;

		ss >> edt.id;
		ss >> edt.cn_name;
		ss >> edt.en_name;
		ss >> edt.shape;

		string tmp;
		ss >> tmp;
		edt.color = ParseColor(tmp);

		effectdata.push_back(edt);
		++i;
	}
	XCPT_R
}

#define EFF_FS (40*(uiy/1080.0))
#define EFF_X_GAP (EFF_FS*0.4)
#define EFF_Y_GAP (EFF_FS*0.5)

#define EFF_INTOX_HURT_CD 2000
class Creature;
class Effect {
public:
	EFFECT_ID id;
	DWORD duration;
	clock_t start;
	clock_t last;

	Effect()
	{
		id = 0, duration = 0, start = last = clock();
	}
	inline bool IsActive() const
	{
		return (clock() - start <= duration);
	}
	inline DWORD GetRemainingTime() const
	{		//ms
		return Clamp<long>(duration - (clock() - start),0, duration);
	}
	static string GetTimeString(DWORD dwMs)
	{
		if (dwMs <= 0)
			return "now";
		if (dwMs < 1000)
			//return ToString(dwMs) + "ms";
			return "0s";
		long sec = dwMs / 1000;
		long mi = sec / 60;
		long h = mi / 60;
		if (mi < 1)
			return ToString(sec) + "s";
		if (h < 1)
			return ToString(mi) + "m" + ToString(sec % 60) + "s";
		return ToString(h) + "h" + ToString(mi % 60) + "m" + ToString(sec % 60) + "s";
	}
	template <typename T>
	void Update(T* cptr);
	void Draw(long x, long y) const
	{
		string shape = effectdata.at(id).shape;
		color_t color = effectdata.at(id).color;
		DWORD ms = GetRemainingTime();
		string tms = GetTimeString(ms);
		setfont(EFF_FS, 0, "Determination Mono");
		setcolor(color);
		pxprint(x, y, shape);
		setfont(EFF_FS*0.8, 0, "Determination Mono");
		setcolor(WHITE);
		pxprint(x, y + EFF_FS + 1, tms);
	}
};
Effect MakeEffect(EFFECT_ID eid, DWORD duration)
{
	Effect eff;
	eff.id = eid;
	eff.duration = duration;
	eff.start = clock();
	return eff;
}
Effect MakeEffect(const string& en_name, DWORD duration)
{
	EFFECT_ID id = 0;
	for (size_t i = 0; i < effectdata.size(); ++i)
		if (effectdata.at(i).en_name == en_name)
		{
			id = effectdata.at(i).id;
			break;
		}
	return MakeEffect(id, duration);
}

class Creature
{	//base class
public:
	long uuid;
	long x;
	long y;
	DIR dir;

	short hp;
	vector<Effect> effects;

	clock_t lastMove;
	clock_t lastAttack;

	Creature()
	{
		uuid = INVALID_UUID;
		x = y = 0;
		hp = 0;
		lastMove = lastAttack = clock();
	}
	inline bool HaveEffect(EFFECT_ID effid) const
	{
		for (const auto& eff : effects)
			if (eff.id == effid)
				return true;
		return false;
	}
	void AddEffect(const Effect& effect)
	{
		if (effect.id == 0)	return;
		for (auto& eff : effects)
		{
			if (eff.id == effect.id)
			{	//持续时间取较高值
				if (eff.GetRemainingTime() < effect.duration)
				{
					eff.duration = effect.duration;
					eff.start = clock();
				}
				return;
			}
		}
		effects.push_back(effect);
	}
	void AddEffect(EFFECT_ID effid, DWORD duration)
	{
		AddEffect(MakeEffect(effid, duration));
	}
	void AddEffect(const string& eff_en_name, DWORD duration)
	{
		AddEffect(MakeEffect(eff_en_name, duration));
	}
	void ClearEffect(EFFECT_ID effid)
	{
		size_t k = 0;
		for (const auto& eff : effects)
		{
			if (eff.id == effid)
			{
				auto iter = effects.begin();
				size_t m = 0;
				while (iter != effects.end() && m != k) {
					iter++;
					m++;
				}
				iter = effects.erase(iter);
				continue;
			}
			++k;
		}
	}
#define EFF_SWIFT_MOVE_CD_RATIO 0.75
#define EFF_SLOW_MOVE_CD_RATIO 1.33
#define EFF_COLD_MOVE_CD_RATIO 1.45
	double FXMoveCDCoefficient(void) const
	{
		double coe = 1.0;
		if (HaveEffect(1))
			coe *= EFF_SWIFT_MOVE_CD_RATIO;
		if (HaveEffect(2))
			coe *= EFF_SLOW_MOVE_CD_RATIO;
		if (HaveEffect(5))
			coe *= EFF_COLD_MOVE_CD_RATIO;
		return coe;
	}
	//virtual void Hurt(short damage, bool isFXHurt);
	template<typename T>
	void UpdateFX(T* who)
	{
		size_t k = 0;
		for (auto& eff : effects)
		{
			if (!eff.IsActive())
			{
				auto iter = effects.begin();
				size_t m = 0;
				while (iter != effects.end() && m != k) {
					iter++;
					m++;
				}
				iter = effects.erase(iter);
				continue;
			}
			eff.Update(who);
			++k;
		}
	}
};

//Entity Status
#define EST_ALIVE 0
#define EST_DEAD 1

#define ET_SAY_CD 12000
#define ET_HURT_TIME 350
#define ET_DEATH_TIME 300
class Entity : public Creature{	//实体
	public:
		ENTITY_ID id;
		
		AI_TYPE ai;		//current ai

		JSON tag;
		short status;
		clock_t start;
		clock_t lastHurt;
		clock_t lastDeath;
		clock_t lastSay;	//Idle Say Timestamp
		clock_t lastStepSnd;
		clock_t lastIdleRefresh;

		Entity() {
			x = y = 0;
			dir = RIGHT;
			id = 0;
			uuid=-1;
			hp = 0;
			lastMove = clock();
			lastHurt = clock() - ET_HURT_TIME;
			lastSay = clock();
			lastStepSnd = clock();
			status = EST_ALIVE;
			lastIdleRefresh = clock();
			start = clock();
		}
		inline ENTITY_TYPE GetType(void)	const {
			return entitydata.at(id).type;
		}
		inline DPID GetDPID(void) const
		{
			return entitydata.at(id).GetDPID();
		}
		bool IsAlive(void){
			if ((hp <= 0 || entitydata.at(id).data.HaveKey("Life") 
				            && (clock() - start > entitydata.at(id).data.GetLong("Life")))
				&& status == EST_ALIVE)
			{	//刚死
				lastDeath = clock();
				return false;
			}
			return (hp > 0 && status == EST_ALIVE 
				&& (!entitydata.at(id).data.HaveKey("Life")
					|| (clock() - start <= entitydata.at(id).data.GetLong("Life"))));
		}
		inline bool TimeToBeCleared(void)
		{	//怪物和动物有死亡动画哦
			if (!IsAlive() && GetType() != ETT_MONSTER && GetType() != ETT_ANIMAL)
				return true;
			return !IsAlive() && (clock() - lastDeath > ET_DEATH_TIME);
		}
		inline bool TimeToMove(void)	const
		{
			if (ai == AI_STILL)	return false;
			return (clock() - lastMove >= GetMoveCD());
		}
		inline bool IsBeingHurt(void)	const
		{
			return (clock() - lastHurt < ET_HURT_TIME);
		}
#define ET_STEPSND_CD 5000
		void StepSound(void)
		{
			if (clock() - lastStepSnd <= ET_STEPSND_CD)
				return;
			lastStepSnd = clock();
			if (!entitydata.at(id).data.HaveKey("AudioFolderName"))	return;
			string foldername = entitydata.at(id).data.GetString("AudioFolderName");
			vector<string> fnames = GetDirFiles(AUDIO_DIR + "entity\\" + foldername + "\\", "step*.mp3");
			if (fnames.empty())
			{
				return;
			}
			SND("entity\\" + foldername + "\\step" + ToString(RandomRange(1, fnames.size())));
		}
		void SaySound(void)
		{
			if (!entitydata.at(id).data.HaveKey("AudioFolderName"))	return;
			string foldername = entitydata.at(id).data.GetString("AudioFolderName");
			vector<string> fnames = GetDirFiles(AUDIO_DIR + "entity\\" + foldername + "\\", "say*.mp3");
			if (fnames.empty())
			{
				return;
			}
			SND("entity\\" + foldername + "\\say" + ToString(RandomRange(1, fnames.size())));
		}
		void HurtSound(void)
		{
			if (!entitydata.at(id).data.HaveKey("AudioFolderName"))	return;
			string foldername = entitydata.at(id).data.GetString("AudioFolderName");
			vector<string> fnames = GetDirFiles(AUDIO_DIR + "entity\\" + foldername + "\\", "hurt*.mp3");
			if (fnames.empty())
			{
				SaySound();
				return;
			}
			SND("entity\\" + foldername + "\\hurt" + ToString(RandomRange(1, fnames.size())));
		}
		void DeathSound(void)
		{
			if (!entitydata.at(id).data.HaveKey("AudioFolderName"))	return;
			string foldername = entitydata.at(id).data.GetString("AudioFolderName");
			vector<string> fnames = GetDirFiles(AUDIO_DIR + "entity\\" + foldername + "\\", "death*.mp3");
			if (fnames.empty())
			{
				HurtSound();
				return;
			}
			SND("entity\\" + foldername + "\\death" + ToString(RandomRange(1, fnames.size())));
		}
		inline vector<short> GetAttackingModes(void) const
		{
			return entitydata.at(id).GetAttackModes();
		}
		inline bool IsLightSource(void) const
		{	//Entit y::
			return entitydata.at(id).IsLightSource();
		}
		inline LIGHTNESS GetOwnLightness(void) const
		{
			return entitydata.at(id).GetOwnLightness();
		}
		inline bool CanCloseAttack(void) const
		{
			return entitydata.at(id).CanCloseAttack();
		}
		inline bool CanFarAttack(void) const
		{
			return entitydata.at(id).CanFarAttack();
		}
		inline bool CanSuchAttack(short atm) const
		{
			return entitydata.at(id).CanSuchAttack(atm);
		}
		void SetOwnerUUID(long newuuid)
		{
			if(!tag.HaveKey("Owner"))
				tag.AddItem("Owner",ToString(newuuid)+"L");
			else
				tag.SetValueRaw("Owner",(ToString(newuuid)+"L").c_str());
		}
		short GetRStep(void) const
		{
			short rstep=0;
			bool suc = tag.GetSimpleValue<short>("MoveCD",&rstep);
			if(!suc)
			{
				ErrorLogTip(NULL,"无法获取直线运动的实体剩余步数 en_name="+entitydata.at(id).en_name,"Entity::GetRStep");
				return 0;
			}
			return rstep;
		}
		void SetRStep(short rstep)
		{
			if(!tag.HaveKey("RStep"))
			{
				tag.AddItem("RStep",ToString(rstep)+"s");
			}else{
				tag.SetShort("RStep",rstep);
			}
		}
		void DropLoot();
		clock_t GetMoveCD(void) const;
		short GetAttack(void) const
		{
			short at=0;
			if(ai == AI_LINE || ai == AI_STILL)
			{	//飞行弹药伤害数据在Entity对象内
				bool suc = tag.GetSimpleValue<short>("Attack",&at);
				if(!suc)
				{
					ErrorLogTip(NULL,"无法获取直线运动的实体的攻击伤害 en_name="+entitydata.at(id).en_name,"Entity::GetMoveCD");
					return 0;
				}
				return at;
			}else if(ai == AI_CHASE || ai == AI_FLEE || ai == AI_IDLE)
			{	//生物伤害数据在EntityData对象中
				bool suc = entitydata.at(id).data.GetSimpleValue<short>("Attack",&at);
				if(!suc)
				{
					ErrorLogTip(NULL,"无法获取直线运动的实体的攻击伤害 en_name="+entitydata.at(id).en_name,"Entity::GetMoveCD");
					return 0;
				}
				return at;
			}
			ErrorLogTip(NULL,"无效的AI类型="+ToString(ai),"Entity::GetAttack");
			return 0;
		}
		short GetRange(void)	const
		{	//注意：仅对远战怪物使用该方法 
			short rg=0;
			bool suc = entitydata.at(id).data.GetSimpleValue<short>("Range",&rg);
			if(!suc)
			{
				ErrorLogTip(NULL,"无法获取实体射程 en_name="+entitydata.at(id).en_name,"Entity::GetRange");
				return 0;
			}
			return rg;
		}
		KBLEVEL GetKnockbackLevel(void)	const
		{
			if (GetType() == ETT_BLADE || IsProjectileET(GetType()))
				return tag.GetShort("KnockbackLevel");
			if (!entitydata.at(id).data.HaveKey("KnockbackLevel"))
				return 0;	//no kb
			short kb = 0;
			bool suc = entitydata.at(id).data.GetSimpleValue<short>("KnockbackLevel", &kb);
			if (!suc)
			{
				ErrorLogTip(NULL, "无法获取实体击退力 en_name=" + entitydata.at(id).en_name, "Entity::GetKnockbackLevel");
				return 0;
			}
			return kb;
		}
		short GetFarAttack(void) const
		{
			short at=0;
			bool suc = entitydata.at(id).data.GetSimpleValue<short>("FarAttack",&at);
			if(!suc)
			{
				ErrorLogTip(NULL,"无法获取实体远程攻击伤害 en_name="+entitydata.at(id).en_name,"Entity::GetFarAttack");
				return 0;
			}
			return at;
		}
		ENTITY_ID GetProjID(void) const
		{
			short eid=0;
			bool suc = entitydata.at(id).data.GetSimpleValue<short>("ProjID",&eid);
			if(!suc)
			{
				ErrorLogTip(NULL,"无法获取实体远程投掷物ID en_name="+entitydata.at(id).en_name,"Entity::GetProjID");
				return 0;
			}
			return eid;
		}
		clock_t GetProjMoveCD(void) const
		{
			short cd=0;
			bool suc = entitydata.at(id).data.GetSimpleValue<short>("ProjMoveCD",&cd);
			if(!suc)
			{
				ErrorLogTip(NULL,"无法获取实体远程投掷物移动冷却 en_name="+entitydata.at(id).en_name,"Entity::GetProjMoveCD");
				return 0;
			}
			return cd;
		}
		long GetOwnerUUID(void) const
		{
			long uuid=0;
			bool suc = tag.GetSimpleValue<long>("Owner",&uuid);
			if(!suc)
			{
				ErrorLogTip(NULL,"无法获取实体主人UUID! en_name="+entitydata.at(id).en_name,"Entity::GetOwnerUUID");
				return uuid;
			}
			return uuid;
		}
		inline short GetMaxHp(void) const
		{
			return entitydata.at(id).GetMaxHp();
		}
		void Die(bool dropLoot = true)
		{
//			DebugLog("Die type="+ToString(GetType()));
			if(GetType() == ETT_MONSTER || GetType() == ETT_ANIMAL)
			{
				if(dropLoot)
					DropLoot();
			}
			hp = 0;
			lastDeath = clock();
			status = EST_DEAD;
		}
		void Hurt(short damage, bool isFXHurt=false)
		{	//Entit y::Hurt
			if (!IsAlive())
				return;
			hp -= damage;
			CreateDamageIndicator(x, y, damage, isFXHurt?DIS_EFFECT:DIS_HARM, hp <= 0);
			lastHurt=clock();
			if(hp < 0)
			{
				hp = 0;
			}
			if(hp == 0)
				  Die();
		}
		inline clock_t GetAttackCD(void) const
		{
			return entitydata.at(id).GetAttackCD();
		}
		inline bool TimeToAttack(void) const
		{
			return (clock() - lastAttack >= GetAttackCD());
		}
		void Attack();
		void CheckDeath();
		void Move();
		void AdaptData() 		throw(out_of_range) {
			XCPT_L
			ai = entitydata.at(id).GetAIType();
			if(!IsProjectileET(GetType()) && GetType() != ETT_ITEM && GetType() != ETT_BLADE)
			{
				hp = GetMaxHp();
			}else{
				hp = 1;
			}
			if (entitydata.at(id).data.HaveKey("IsRandomColor")
				&& entitydata.at(id).data.GetShort("IsRandomColor")==1)
			{
				if (tag.HaveKey("ColorIndex"))
					tag.SetShort("ColorIndex", RandomRange(0, entitydata.at(id).colors.size()));
				else
					tag.AddItem("ColorIndex", ToString(RandomRange(0, entitydata.at(id).colors.size())) + "s");
				//DebugLog("Colorindex="+ToString(tag.GetShort("ColorIndex"))+" Colorsize="+ToString(entitydata.at(id).colors.size()));
			}
			XCPT_R
		}
		void SetMoveCD(short mc)
		{
			if(!tag.HaveKey("MoveCD"))
				tag.AddItem("MoveCD",ToString(mc)+"s");
			else
				tag.SetShort("MoveCD",mc);
		}
		short GetShapeIndex(void)	const {
			if (tag.HaveKey("ShapeIndex"))
			{
				short index = tag.GetShort("ShapeIndex");
				if (index >= entitydata.at(id).shapes.size())
				{
					ErrorLogTip(NULL, "ShapeIndex越界", "Entity::GetShapeIndex");
					index = 0;
				}
				return index;
			}
			return 0;
		}
		short GetColorIndex(void)	const {
			if (tag.HaveKey("ColorIndex"))
			{
				short index = tag.GetShort("ColorIndex");
				//DebugLog("index=" + ToString(index) + " tag=" + tag.ToString());
				if (index >= entitydata.at(id).colors.size())
				{
					ErrorLogTip(NULL, "ColorIndex越界", "Entity::GetColorIndex");
					index = 0;
				}
				return index;
			}
			return 0;
		}
		void Update() 
		{	//Entit y::
			Move();
			Attack();
			PlayerInteraction();	//实体与玩家的交互 
			CheckDeath();
		}
		bool GetItemData(ITEM_ID& id, short& cnt, JSON* itag)	const 
		{	//Entit y::
			XCPT_L
			if(!tag.HaveKey("Item")) {
				ErrorLogTip(NULL,"掉落物缺失{Item}标签","Entity::GetItemData");
				return false;
			}
			short iid=0;
			bool suc = tag.GetSimpleValue<short>("Item",&iid);
			if(!suc) {
				ErrorLogTip(NULL,"无法获取掉落物物品ID数据","Entity::GetItemData");
				return false;
			}
			id = ITEM_ID(iid);
			if(!tag.HaveKey("Count")) {
				cnt = 1;
			} else {
				suc = tag.GetSimpleValue<short>("Count",&cnt);
				if(!suc) {
					ErrorLogTip(NULL,"无法获取掉落物物品数量数据","Entity::GetItemData");
					return false;
				}
			}
			if(itag != nullptr)
			{
				if(tag.HaveKey("tag"))
				{
					suc = tag.GetComplexValue("tag",itag);
					if(!suc) {
						ErrorLogTip(NULL,"无法获取掉落物物品标签","Entity::GetItemData");
						return false;
					}
				}
			}
			XCPT_R
			return true;
		}
		#define BPOS_CNT 9
		void RandomBlockPos(void)
		{	//iter->RandomBlockP os出现崩溃的现象，不清楚为什么 
			XCPT_L
			TrackLog("...");
			if(tag.HaveKey("BlockPos"))
				return;
			tag.AddItem("BlockPos",ToString(RandomRange(0,BPOS_CNT-1))+"s");
			XCPT_R
		}
		bool GetBlockPos(short* bpos) const
		{
			XCPT_L
			if(!tag.HaveKey("BlockPos"))	return false;
			return tag.GetSimpleValue<short>("BlockPos",bpos);
			XCPT_R
		}
		void BlockPosOffset(int& scr_x, int& scr_y, short bpos) const
		{
			scr_x += ((bpos % 3 - 1) * 1/3.0) * g_block_px;
			scr_y += ((bpos / 3 - 1) * 1/3.0) * g_block_px;
		}
		void PlayerInteraction();
		void Draw();
};
vector<Entity> entities;

struct EntityDefaultJSONData {
	ENTITY_ID id;
	JSON json;

	EntityDefaultJSONData() {
		id=0;
	}
};
vector<EntityDefaultJSONData> edjsondata;
void ReadEntityDefaultJSONData() {
	if(!ExistFile(EDJSONDATA_PATH)) {
		ErrorLogTip(NULL,"实体默认JSON数据丢失","ReadEntityDefaultJSONData");
		return;
	}
	DebugLog("【读取】读取实体默认JSON数据："+EDJSONDATA_PATH);
	vector<string> lines = ReadFileLines(EDJSONDATA_PATH);
	long i=1;
	while(i < lines.size()) {
		EntityDefaultJSONData edjd;
		stringstream ss;
		ss<<lines.at(i);

		ss>>edjd.id;

		string tmp,tmptmp;
		ss>>tmp;
		short c=0;
		while(!ss.eof()) {
			ss>>tmp;
			if(c!=0)
				tmptmp+=" ";
			tmptmp+=tmp;
			c++;
		}
//		DebugLog(tmptmp);

		edjd.json = ParseJSON(tmptmp,"ReadEntityDefaultJSONData");
		edjsondata.push_back(edjd);
		++i;
	}
}
JSON GetDefaultEntityJSON(ENTITY_ID id) {
	for(short j = 0; j < edjsondata.size(); ++j)
		if(edjsondata.at(j).id == id)
			return edjsondata.at(j).json;
//	ErrorLogTip(NULL,"无法根据实体ID获取默认JSON数据","GetDefaultEntityJSON");
	return JSON();
}
vector<Entity>::iterator SpawnEntity(ENTITY_ID id, long x, long y, DIR dir, const char* etag=nullptr) {
	XCPT_L
	Entity et;
	et.id = id;
	et.x = x;
	et.y = y;
	et.dir = dir;
	et.uuid = AttainNewUUID();
	et.status = EST_ALIVE;
	et.lastMove=clock();
	et.lastAttack=clock();
	if(etag != nullptr)
		et.tag = ParseJSON(etag,"SpawnEntity");
	else {
		et.tag = GetDefaultEntityJSON(id);
	}
	et.AdaptData();
	//DebugLog("SpawnEntity: id="+ToString(id)+" x="+ToString(x)+" y="+ToString(y)+" uuid="+ToString(et.uuid)+" tag="+et.tag.ToString());
	et.start = clock();
	entities.push_back(et);
	return entities.end();
	XCPT_R
}
void SpawnItem(long x, long y, const char* etag=nullptr)
{
	SpawnEntity(1,x,y,RIGHT,etag);
	entities.at(entities.size()-1).RandomBlockPos();
}
void SpawnItem(long x, long y, ITEM_ID id, short cnt, const char* itag/*=nullptr*/)
{
	string etag="{Item:"+ToString(id)+"s,Count:"+ToString(cnt)+"s";
//	if(itemdata.at(id).data.HaveKey("Durability"))
//	{
//		short dur = itemdata.at(id).GetDurability();
//	}
	if(itag != nullptr)
	{
		etag += ",tag:";
		etag += itag;
	}
	etag += "}";
//	DebugLog("SpawnItem etag="+etag);
	SpawnEntity(1,x,y,RIGHT, etag.c_str());
	entities.at(entities.size()-1).RandomBlockPos();
}
void SpawnProjectile(ENTITY_ID id, long x, long y, DIR dir, long owner_uuid, short tot_attack, short move_cd, short range, KBLEVEL kblvl)
{
	string tag = "{Owner:"+ToString(owner_uuid)+"L,Attack:"+ToString(tot_attack)+"s,MoveCD:"+ToString(move_cd)+"s,KnockbackLevel:"+ToString(kblvl)+",RStep:"+ToString(range)+"s}";
	SpawnEntity(id,x,y,dir,tag.c_str());
//	DebugLog(entities.at(entities.size()-1).tag.ToString());
}

enum BIOME_ID : USHORT {
	biome_void = 0,
	biome_pine_forest,
	biome_poplar_forest,
	biome_camphor_forest,
	biome_ginkgo_forest,
	biome_taiga,
	biome_grass,
	biome_desert,
	biome_stones,
	biome_cave,
	biome_mountain,
	biome_rocks,
	biome_iceland,
	biome_jungle,
	biome_swamp,
	biome_ocean,
	biome_dungeon,
	biome_bamboo_forest,
	biome_lake,
	biome_apple_forest,
	biome_pear_forest,
};
class BiomeEntitySpawnData;
void SpawnEntityNearby(ENTITY_ID id, BIOME_ID biome_id, const BiomeEntitySpawnData& esdata);
class EntitySpawning
{	//刷怪项目
	public:
		ENTITY_ID id;
		clock_t spawn_min;
		clock_t spawn_max;
		clock_t cur_spawn_cd;
		clock_t lastSpawn;

		BiomeEntitySpawnData* pdata = nullptr;

		EntitySpawning()
		{
			id = 0;
			spawn_min = 2000;
			spawn_max = 3000;
			cur_spawn_cd = 2500;
			lastSpawn = clock();
		}
		inline void ConsiderNext()
		{
			cur_spawn_cd = RandomRange(spawn_min, spawn_max);
		}
		inline bool TimeToSpawn() const
		{
			return (clock() - lastSpawn >= cur_spawn_cd);
		}
		bool CanSpawnNow(void) const;
};
class BiomeEntitySpawnData
{	//群落刷怪数据
public:
	ENTITY_ID id;
	clock_t spawn_min;
	clock_t spawn_max;

	short timeLimit;	//0-nope 1-only night 2-only daytime
	short allowInWater;		//0-no water 1-allow
	BiomeEntitySpawnData()
	{
		id = 0;
		spawn_min = 2000;
		spawn_max = 3000;
		timeLimit = 1;	//default only night
		allowInWater = 1;
	}
	void Parse(const string& data)
	{
		JSON json = ParseJSON(data, "BiomeEntitySpawnData::Parse");
		if (json.HaveKey("id"))
		{
			short sh=0;
			json.GetSimpleValue<short>("id", &sh);
			id = sh;
		}
		if (json.HaveKey("spmin"))
		{
			json.GetSimpleValue<long>("spmin", &spawn_min);
		}
		if (json.HaveKey("spmax"))
		{
			json.GetSimpleValue<long>("spmax", &spawn_max);
		}
		if (json.HaveKey("timeLimit"))
		{
			json.GetSimpleValue<short>("timeLimit", &timeLimit);
		}
		if (json.HaveKey("allowInWater"))
		{
			json.GetSimpleValue<short>("allowInWater", &allowInWater);
		}
	}
	EntitySpawning Start()
	{
		EntitySpawning espawn;
		espawn.id = id;
		espawn.lastSpawn = clock();
		espawn.spawn_min = spawn_min;
		espawn.spawn_max = spawn_max;
		espawn.ConsiderNext();
		espawn.pdata = this;
		return espawn;
	}
};

#define ESPAWN_MIN_DISTANCE (PL_VIEW_W_HALF+1)
#define ESPAWN_MAX_DISTANCE int(PL_VIEW_W_HALF*2.8)
#define ESPAWNER_WAIT_TIME 20000
class EntitySpawner
{	//群落刷怪器
public:
	BIOME_ID biome_id;
	vector<EntitySpawning> espawns;
	clock_t lastPlayerExit;

	EntitySpawner()
	{
		lastPlayerExit = -1;
	}
	inline bool TimeToClose() const
	{	//玩家离开群落一定时长后本对象销毁
		return (lastPlayerExit != -1 && clock() - lastPlayerExit >= ESPAWNER_WAIT_TIME);
	}
	void DoSpawn()
	{
		for (int i = 0; i < espawns.size(); ++i)
		{
			if (espawns.at(i).CanSpawnNow())
			{	//刷怪
				SpawnEntityNearby(espawns.at(i).id, biome_id, *espawns.at(i).pdata);
				espawns.at(i).ConsiderNext();
				espawns.at(i).lastSpawn = clock();
			}
		}
	}
};

//16的倍数
#define MAP_X_MIN 800
#define MAP_X_MAX 1200
#define MAP_Y_MIN 800
#define MAP_Y_MAX 1200



ITEM_ID BlockToItem(BLOCK_ID bid);
typedef short LIGHT_LVL;	//1~16 当前发光亮度等级 
#define NORMAL_LIGHT_LVL 10		//正常值 
color_t CJZAPI LightnessBlockColor(color_t orig, LIGHT_LVL lvl)
{
	constexpr float min_ratio = 0.02f;
	auto r = EGEGET_R(orig);
	auto g = EGEGET_G(orig);
	auto b = EGEGET_B(orig);
	r *= (min_ratio + (lvl - 1) / float(NORMAL_LIGHT_LVL - 1) * (1.0f - min_ratio));
	g *= (min_ratio + (lvl - 1) / float(NORMAL_LIGHT_LVL - 1) * (1.0f - min_ratio));
	b *= (min_ratio + (lvl - 1) / float(NORMAL_LIGHT_LVL - 1) * (1.0f - min_ratio));
	return EGERGB(r,g,b);
}
inline color_t CJZAPI RateColor(color_t orig, float rate)
{
	return EGERGB(Clamp(int(EGEGET_R(orig)*rate),0,255), Clamp(int(EGEGET_G(orig)*rate),0,255), Clamp(int(EGEGET_B(orig)*rate),0,255));
}
class Item;
inline string BlockTagForItemTag(const string& sjson)
{
	return "{BlockTag:" + sjson + "}";
}
BLOCK_ID RandomGravestone(void)
{	//随机墓碑 
	static vector<BLOCK_ID> gravestones;
	static bool inited=false;
	if(gravestones.empty() && !inited)
	{
		for(BLOCK_ID j = 0; j < blockdata.size(); ++j)
		{
			if(blockdata.at(j).HaveClass(BCLASS_GRAVESTONE))
				gravestones.push_back(j);
		}
		inited = true;
	}
	return Choice(gravestones);
}
inline LIGHTNESS BurnerLightness(short fuelVal)
{
	return LIGHTNESS(4.5f + (-225.0f / (fuelVal + 50.0f)));
}
class Block {	//方块类
	public:
		BLOCK_ID id;
		//	BLOCK_ID id2;	//不打算使用 也不存储至存档
		short dur;
		BIOME_ID biome;	//生物群系ID
		LIGHT_LVL light;	//当前亮度 

		color_t color;
		char* shape;
		JSON tag;	//数据存储于此 越简洁越好

		Block() {
			id=0;
			dur=1;
			biome = biome_void;
			color=WHITE;
			shape=nullptr;
			light=1;
		}
		inline void Normalize() {
			if(!HaveClass(BCLASS_LIQUID) //非液体
			        && id > 0 && dur <= 0) {
				id = 0;
				dur = 0;
				tag.Clear();
			}
		}
		inline void Clear() {
			id = 0;
			dur = 0;
			light = 1;
			tag.Clear();
			color=WHITE;
			shape=nullptr;
		}
		#define BURNER_RED_FUEL_VALUE 120
		double GetBurnerRedRate(void) const
		{
			double rate = (GetFuel() / double(BURNER_RED_FUEL_VALUE));
			if(rate > 1.0)	rate = 1.0;
			return rate;
		}
		#define EDROP_COLOR_DARKNESS_PERCENTAGE 0.13
		void AdjustShape(void) {
			short index=0;	//0是默认形态 
			if(HaveClass(BCLASS_DOOR)) {
				if (!tag.HaveKey("Open")) {
					//abnormal
					tag.AddItem("Open", "0s");
				}
				string type = tag.GetValueType("Open", "Block::AdjustShape");
				if(type != "short") {
					ErrorLogTip(NULL,"{Open}标签数据类型不是short 而是"+type,"Block::AdjustShape");
					return;
				}
				short ret=0;
				tag.GetSimpleValue<short>("Open",&ret);
				if(ret!=1)
					index = 1;
			}else if(HaveClass(BCLASS_FRUITFUL) && HaveFruit())
			{	//注意这里开花结果逻辑错综复杂 
				if(HaveClass(BCLASS_BLOSSOM))
					index=2;	//可开花，在结果 
				else
				    index=1;
			}else if(HaveClass(BCLASS_BLOSSOM) && IsBlooming())
			{
				index=1;
			}
			this->color = blockdata.at(id).colors[index];
			if(HaveClass(BCLASS_BURNER) && blockdata.at(id).colors.size() > 1/* && GetBurnerRedRate()>0.0*/)
			{
				this->color = StepColor(blockdata.at(id).colors[0], blockdata.at(id).colors[1], GetBurnerRedRate());
			}
			this->shape = (char*)(blockdata.at(id).shapes[index].c_str());
			if(tag.HaveKey("EDrop"))
				this->color = StepColor(this->color, BLACK, EDROP_COLOR_DARKNESS_PERCENTAGE);
		}
		void Reset(BLOCK_ID _id, BIOME_ID _biome, const JSON& _tag) {
			id = _id;
			if(_biome!=biome_void)
				biome = _biome;
			tag = _tag;
			dur = blockdata.at(id).dur;
			AdjustShape();
		}
		void Reset(BLOCK_ID _id, BIOME_ID _biome=biome_void, const char* _tag=nullptr) {
			id = _id;
			if(_biome!=biome_void)
				biome = _biome;
			if(_tag != nullptr) {
//				DebugLog("id="+ToString(_id)+" _tag="+ToString(_tag));
				tag = ParseJSON(_tag,"Block::Reset");
			}
			dur = blockdata.at(id).dur;
			AdjustShape();
		}
		bool IsBlooming(void)	const
		{
			if(!tag.HaveKey("Blossom"))	return 0;
			short blo=0;
			BOOL bRet = tag.GetSimpleValue<short>("Blossom",&blo);
			return bRet && blo;
		}
		short GetFuel(void) const
		{
			if(!tag.HaveKey("Fuel"))	return 0;
			short f=0;
			bool suc = tag.GetSimpleValue<short>("Fuel",&f);
			if(!suc)
			{
				ErrorLogTip(NULL,"无法获取方块当前燃值 bid="+ToString(id),"Block::GetFuel");
				return 0;
			}
			return f;
		}
		void SetFuel(short newFuelVal)
		{
			if(!tag.HaveKey("Fuel"))
			{
				tag.AddItem("Fuel",ToString(newFuelVal)+"s");
			}else{
				tag.SetShort("Fuel",newFuelVal);
			}
		}
		inline bool HasFuel(void) const
		{
			return (GetFuel()>0);
		}
		inline void AddFuel(short addition)
		{	//添加燃料 
			SetFuel(GetFuel() + addition);
			AdjustShape();
		}
		inline void ReduceFuel(short pos_val)
		{	//减少燃料 
			short v = GetFuel();
			v -= pos_val;
			if(v < 0)	v = 0;
			SetFuel(v);
			AdjustShape();
		}
		inline short GetDurLevel(void)	const {
			return blockdata.at(id).dur_lvl;
		}
		inline LIGHTNESS GetOwnLightness(void)	const
		{
			if(HaveClass(BCLASS_BURNER))
			{	//烧火器具的亮度根据燃料决定 
				return BurnerLightness(GetFuel());
			}
			return blockdata.at(id).GetOwnLightness();
		}
		Item GetFruit(void) const;
		bool HaveFruit(void) const
		{
			if(!tag.HaveKey("Fruit"))	return false;
			string raw;
			BOOL bret = tag.GetSimpleValue<string>("Fruit",&raw);
			if(!bret)	return false;
			string fruitstr = UncoverValue(raw,"string");
			if(fruitstr.empty())	return false;
			return true;
		}
		Item PickFruit(void);
		void DropFruit(const long& x,const long& y);
		bool Break(long x, long y);
		inline bool IsKeeperContainer(BLOCK_ID bid = 0) const
		{
			if(bid == 0)	bid = this->id;
			return blockdata.at(bid).IsKeeperContainer();
		}
		bool Damage(short power, short powerlvl, long x, long y) {
			//Block::
			XCPT_L
//			DebugLog("powerlvl="+ToString(powerlvl)+" durlvl="+ToString(GetDurLevel()));
			if(powerlvl < GetDurLevel())	//挖不动
				return false;
			if(HaveClass(BCLASS_LIQUID)	//液体不能挖
			        || HaveClass(BCLASS_AIR)	//气体不能挖
			  )
				return true;
			dur -= power;
			if(dur<=0) {	//被挖掉了
				return Break(x,y);								//broken snd
			}
			else {
				PlayFamilySound(blockdata.at(id).snd_family, SNDFW_DAMAGE);	//damage snd
			}
			XCPT_R
			return true;
		}
		Item& GetContainerItem(short index) const;	//支持get, set 
		inline bool IsEmptyTag(void) const {
			return tag.Empty();
		}
		inline bool IsBarrier(void) {
			//是否是障碍
			if(HaveClass(BCLASS_DOOR)) {
				if(!tag.HaveKey("Open")) {
					//abnormal
					tag.AddItem("Open","0s");
					return true;
				}
				short isOpen=false;
				bool suc = tag.GetSimpleValue<short>("Open",&isOpen);
				if(!suc) {
					ErrorLogTip(NULL,"无法获取门的开关状态数据\nid="+ToString(id),"Block::IsBarrier");
					return true;
				}
				return bool(!isOpen);
			}
			//Other Situations
			return (!HaveClass(BCLASS_AIR) 	//空气类
			        && !HaveClass(BCLASS_PASSABLE) 	//可完全穿过
			        && !HaveClass(BCLASS_LIQUID)	//液体
			       );
		}
		inline bool HaveClass(BLOCK_CLASS_ID bcid) const {
			return blockdata.at(id).HaveClass(bcid);
		}
		inline void LimitLight(void)
		{
			ClampA<LIGHT_LVL>(light,1,16);
		}
		inline void LimitLightLow(void)
		{
			ClampA<LIGHT_LVL>(light,1,NORMAL_LIGHT_LVL);
		}
		void SetLight(LIGHT_LVL lvl)
		{
			light = lvl;
			LimitLight();
		}
		bool AddLight(LIGHT_LVL rel_lvl, bool allowHilite = false)
		{
			light += rel_lvl;
			if(!allowHilite)
			    LimitLightLow();
			else
				LimitLight();
			return (rel_lvl != 0);
		}
		void AbsorbLight(LIGHT_LVL pos_val)
		{
			light -= pos_val;
			LimitLight();
		}
};
struct BlockDefaultJSONData {
	BLOCK_ID id;
	JSON json;

	BlockDefaultJSONData() {
		id=0;
	}
};
vector<BlockDefaultJSONData> bdjsondata;
void ReadBlockDefaultJSONData() {
	if(!ExistFile(BDJSONDATA_PATH)) {
		ErrorLogTip(NULL,"方块默认JSON数据丢失","ReadBlockDefaultJSONData");
		return;
	}
	DebugLog("【读取】读取方块默认JSON数据："+BDJSONDATA_PATH);
	vector<string> lines = ReadFileLines(BDJSONDATA_PATH);
	long i=1;
	while(i < lines.size()) {
		BlockDefaultJSONData bdjd;
		stringstream ss;
		ss<<lines.at(i);

		ss>>bdjd.id;

		string tmp,tmptmp;
		short c=0;
		ss>>tmp;
		while(!ss.eof()) {
			ss>>tmp;
			if(c!=0)
				tmptmp+=" ";
			tmptmp+=tmp;
			c++;
		}
//		DebugLog(tmptmp);

		bdjd.json = ParseJSON(tmptmp,"ReadBlockDefaultJSONData");
		bdjsondata.push_back(bdjd);
		++i;
	}
}

JSON GetDefaultBlockJSON(BLOCK_ID id) {
	for(short j = 0; j < bdjsondata.size(); ++j)
		if(bdjsondata.at(j).id == id)
			return bdjsondata.at(j).json;
//	ErrorLogTip(NULL,"无法根据方块ID获取默认JSON数据","GetDefaultBlockJSON");
	return JSON();
}
inline bool InRange(long ox, long oy, long x1, long y1, long x2, long y2)
{
	return (ox >= x1 && ox <= x2 && oy >= y1 && oy <= y2);
}

#define NBMAP_PX 15
#define NBMAP_W_HALF 20 
#define NBMAP_H_HALF 20
using WP_ID = USHORT;
#define WP_RAIN_THETA (4.26)
#define WP_RAIN_COLOR EGERGB(65,105,225)
#define WP_RAIN_DEF_CD 9
#define WP_SNOW_THETA (4.26)
#define WP_SNOW_COLOR EGERGB(255,250,250)
#define WP_SNOW_DEF_CD 7

#define WPID_NONE 0
#define WPID_RAINDROP 1
#define WPID_SNOWFLAKE 2

#define WPL_BOTTOM 0
#define WPL_MIDDLE 1
#define WPL_TOP 2
struct WeatherParticle {
	WP_ID id;
	USHORT layer;	//0-2
	long x;
	long y;
	long end_y;
	static float rate;

	WeatherParticle()
	{
		id = WPID_NONE;
		x = 0, y = scene_top;
		end_y = scene_bottom + 1;
	}
	void Update()
	{
		long v = GetVelocity();
		x += cos(id == WPID_RAINDROP ? WP_RAIN_THETA : WP_SNOW_THETA) * v;
		y -= sin(id == WPID_RAINDROP ? WP_RAIN_THETA : WP_SNOW_THETA) * v;
	}
	inline long GetVelocity() const
	{
		if (layer == 0)
			return 5 * rate;
		else if (layer == 1)
			return 9 * rate;
		else {
			return 17 * rate;
		}
	}
	inline bool IsActive() const
	{
		return (x >= scene_left && x <= uix && y >= scene_top && y <= scene_bottom && y < end_y);
	}
	void Draw() const;
};
float WeatherParticle::rate = 1.0f;

#define CHUNK_PX 16
#define CHUNKLOAD_MIN_TIME 10000
class Chunk
{			//区块
public:
	long cx;
	long cy;
	vector<Block> blocks;
	clock_t lastLoad;

	Chunk()
	{
		cx = -1, cy = -1;
		lastLoad = clock();
	}
	inline bool AllowToBeUnload(void) const
	{
		return (clock() - lastLoad >= CHUNKLOAD_MIN_TIME);
	}
	inline long GetLeft() const	//world coord
	{
		return cx * CHUNK_PX;
	}
	inline long GetRight() const
	{
		return GetLeft() + CHUNK_PX - 1;
	}
	inline long GetTop() const
	{
		return cy * CHUNK_PX;
	}
	inline long GetBottom() const
	{
		return GetTop() + CHUNK_PX - 1;
	}
	inline DCOORD GetCenter() const
	{
		return DCOORD((GetLeft() + GetRight()) / 2.0, 
				      (GetTop() + GetBottom()) / 2.0);
	}
	inline bool IsIn(const long& x, const long& y) const
	{
		return (x >= GetLeft() && x <= GetRight()
			 && y >= GetTop() && y <= GetBottom());
	}
	void Init()
	{
		if (!blocks.empty())
			blocks.clear();
		Block bair;
		bair.Reset(0);
		for (size_t i = 0; i < CHUNK_PX*CHUNK_PX; ++i)
			blocks.push_back(bair);
	}
	Block& GetBlock(const long& rx, const long& ry, const char* tag="null")
	{
		int index = CHUNK_PX * ry + rx;
		if (index >= blocks.size())
		{
			ErrorLogTip(NULL, "方块访问超界 cx=" + ToString(cx) + " cy=" + ToString(cy) + " rx=" + ToString(rx) + " ry=" + ToString(ry) + " tag=" + ToString(tag));
			return blocks.at(0);
		}
		else if (index >= pow2(CHUNK_PX))
		{
			ErrorLogTip(NULL, "方块访问越界 cx=" + ToString(cx) + " cy=" + ToString(cy) + " rx=" + ToString(rx) + " ry=" + ToString(ry) + " tag=" + ToString(tag));
			return blocks.at(0);
		}
		return blocks.at(index);
	}
};
#define CHUNKLOAD_INI_DIST (CHUNK_PX*8.0)
#define CHUNKLOAD_STD_DIST (PL_VIEW_DIAMETER*11.5)
double g_chunkload_dist=999.0;
inline long ToChunkCoord(const long& world_coord)
{
	return (world_coord / CHUNK_PX);
}
void OrderAddChunk(vector<Chunk>& chunks, const Chunk& chunk)
{
	if (chunks.empty())	chunks.push_back(chunk);
	size_t i = 0;
	auto iter = chunks.begin();
	while (i < chunks.size() && iter != chunks.end() 
		&& chunks.at(i).cx <= chunk.cx 
		&& (chunks.at(i).cx < chunk.cx || chunks.at(i).cy < chunk.cy))
		++i, ++iter;
	chunks.insert(iter, chunk);
}
class ChunkLoader
{			//区块加载器
public:
	long map_w;
	long map_h;
	vector<Chunk> chunks;
	ChunkLoader()
	{
		map_w = map_h = 0;
		g_chunkload_dist = CHUNKLOAD_INI_DIST;
	}
	//区块坐标最值
	inline long GetChunksLeft(void) const
	{
		return 0L;
	}
	inline long GetChunksRight(void) const
	{
		return map_w / CHUNK_PX - 1;
	}
	inline long GetChunksTop(void) const
	{
		return 0L;
	}
	inline long GetChunksBottom(void) const
	{
		return map_h / CHUNK_PX - 1;
	}
	inline void RefreshChunkloadDistance(void)
	{
		g_chunkload_dist = CHUNKLOAD_INI_DIST;
	}
	void SaveChunks()
	{
		if(!ExistFile(LOCAL_SAVE_DIR+"Regions"))
			CreateDirectory((LOCAL_SAVE_DIR + "Regions").c_str(), NULL);
		for (auto& chunk : chunks)
			SaveChunk(chunk);
	}
	void SaveAndClear()
	{
		SaveChunks();
		chunks.clear();
	}
	void InitAllChunks(bool remain)
	{
		size_t wcnt = map_w / CHUNK_PX;
		size_t hcnt = map_h / CHUNK_PX;
		for(size_t _y = 0; _y < hcnt; ++_y)
			for (size_t _x = 0; _x < wcnt; ++_x)
			{
				Chunk chunk;
				chunk.cx = _x;
				chunk.cy = _y;
				chunk.Init();
				SaveChunk(chunk);
				if (remain)
					OrderAddChunk(chunks, chunk);
			}
	}
	void Update(double x, double y)
	{
		for (size_t i{ 0 }; i < chunks.size(); ++i)
		{
			Chunk& chunk = chunks.at(i);
			if (OuttaRange(chunk, x, y) && chunk.AllowToBeUnload())
			{	//离开范围，保存后卸载区块
				//AddChatItem("[{text:\"RemoveChunk: \",color:#white},{text:\"" + ToString(chunk.cx) + "," + ToString(chunk.cy)+"\",color:#pink}]");
				if (chunk.blocks.size() < pow2(CHUNK_PX))
					chunk.Init();
				SaveChunk(chunk, true);
				//Delete the Entities Belonging to that chunk
				size_t k = 0;
				for (const auto& et : entities)
				{
					if (chunk.IsIn(et.x, et.y))
					{
						auto iter2 = entities.begin();
						size_t m = 0;
						while (iter2 != entities.end() && m != k) {
							iter2++;
							m++;
						}
						iter2 = entities.erase(iter2);
						continue;
					}
				}
				//Delete the Chunk
				auto iter = chunks.begin();
				size_t j = 0;
				while (iter != chunks.end() && j != i) {
					iter++;
					j++;
				}
				iter = chunks.erase(iter);
				continue;
			}
			++i;
		}
		//DebugLog("x=" + ToString(x) + " y=" + ToString(y));
		for(double _dy = -g_chunkload_dist; _dy <= g_chunkload_dist; _dy += 2.0)
			for (double _dx = -g_chunkload_dist; _dx <= g_chunkload_dist; _dx += 2.0)
			{
				double abs_x = _dx + x, abs_y = _dy + y;
				long cx = ToChunkCoord(abs_x), cy = ToChunkCoord(abs_y);
				if (!OuttaRange(x, y, CHUNK_PX*cx + CHUNK_PX/2.0 + .5, CHUNK_PX*cy + CHUNK_PX / 2.0 + .5)
					&& OuttaLoadedWorld(CHUNK_PX*cx + CHUNK_PX / 2.0 + .5, CHUNK_PX*cy + CHUNK_PX / 2.0 + .5)
					&& !ChunkOuttaWorld(cx, cy))
				{
					LoadChunk(cx, cy);
				}
			}
	}
	void LoadChunk(long cx, long cy)
	{
		if (ExistChunk(cx, cy))	return;
		Chunk chunk;
		chunk.cx = cx, chunk.cy = cy;
		string path = LOCAL_SAVE_DIR + "Regions\\r";
		path += ToString(cx);
		path += ",";
		path += ToString(cy);
		path += ".bmp";
		//DebugLog("LoadChunk: "+path);
		//AddChatItem("[{text:\"LoadChunk: \",color:#white},{text:\"(" + ToString(cx)+","+ToString(cy) + ")\",color:#green}]");
		fstream fin(path, ios::in);
		if (fin.fail())
		{
			ErrorLogTip(NULL, "加载区块(" + ToString(cx) + "," + ToString(cy) + ")失败：找不到区块数据" + path, "ChunkLoader::LoadChunk");
			return;
		}
		string tmp;
		fin >> tmp;
		size_t i = 0;
		while (!fin.eof() && tmp != "[Entities]") {
			string tmp;
			fin >> tmp;
			if (tmp == "[Entities]")
				break;
			Block b;
			b.id = Convert<string, USHORT>(tmp);
			fin >> b.dur;
			fin >> tmp;
			b.biome = BIOME_ID(Convert<string, u_short>(tmp));
			fin >> tmp;
			if (!tmp.empty())
				b.tag = ParseJSON(tmp, "ChunkLoader::LoadChunk");
			b.AdjustShape();
			chunk.blocks.push_back(b);
			++i;
		}
		chunk.lastLoad = clock();
		OrderAddChunk(chunks, chunk);
		//DebugLog("LoadChunk: loading entities..");
		while (!fin.eof() && fin)
		{
			Entity et;
			fin >> et.id;
			if (fin.eof() || !fin)
				break;
			fin >> et.uuid >> et.ai >> et.x >> et.y >> et.status >> et.dir >> et.hp;
			string tmp;
			fin >> tmp;
			et.tag = ParseJSON(tmp, "ChunkLoader::LoadChunk");
			entities.push_back(et);
		}
		fin.close();
		//DebugLog("LoadChunk over");
	}
	void SaveChunk(Chunk& chunk, bool deleteMobs = false)
	{
		if (chunk.blocks.size() < CHUNK_PX*CHUNK_PX)
			return;
		string path = LOCAL_SAVE_DIR + "Regions\\r";
		path += ToString(chunk.cx);
		path += ",";
		path += ToString(chunk.cy);
		path += ".bmp";
		//DebugLog("SaveChunk: " + path);
		//AddChatItem("[{text:\"SaveChunk: \",color:#white},{text:\"" + ToString(chunk.cx) + "," + ToString(chunk.cy) + "\",color:#purple}]");
		fstream fout(path,ios::out);
		fout << "[ChunkBlocks]" << endl;

		for (long _y = 0; _y < CHUNK_PX; ++_y)
			for (long _x = 0; _x < CHUNK_PX; ++_x) {
				Block& bcref = chunk.GetBlock(_x, _y, "ChunkLoader::SaveChunk");
				if (bcref.tag.HaveKey("Fuel") && bcref.tag.GetShort("Fuel") == 0)
					bcref.tag.RemoveItem("Fuel");	//节省磁盘空间
				fout << bcref.id << " " << bcref.dur << " " << int(bcref.biome) << " " << bcref.tag.ToString() << "	";
				/*if(i % SAVEWORLD_NEWLINE_BLOCK_CNT == 0)
					fout<<endl;		//定期换行 */
			}
		fout << endl;
		fout << "[Entities]" << endl;
		for (long i = 0; i < entities.size(); ++i)
		{
			if (chunk.IsIn(entities.at(i).x, entities.at(i).y))
			{
				if (deleteMobs && entities.at(i).GetType() == ETT_MONSTER)
					continue;		//移除卸载区块的全部怪物
				fout << entities.at(i).id << " " << entities.at(i).uuid << " " << entities.at(i).ai
					<< " " << entities.at(i).x << " " << entities.at(i).y << " " << entities.at(i).status << " " << entities.at(i).dir
					<< " " << entities.at(i).hp << " " << entities.at(i).tag.ToString() << endl;
			}
		}

		fout.close();
	}
	inline bool OuttaRange(double x1, double y1, double x2, double y2) const
	{
		return Distance(x1, y1, x2, y2) > g_chunkload_dist;
	}
	inline bool OuttaRange(const Chunk& chunk, double x, double y) const
	{
		return Distance(chunk.GetCenter().X, chunk.GetCenter().Y, x, y) > g_chunkload_dist;
	}
	inline bool ChunkOuttaWorld(const long& cx, const long& cy) const
	{
		return (cx < GetChunksLeft() || cx > GetChunksRight()
			|| cy < GetChunksTop() || cy > GetChunksBottom());
	}
	bool ExistChunk(const long& cx, const long& cy) const
	{
		if (chunks.empty())	return false;
		//DebugLog("ExistChunk called");
		//Binary Search
		long L = 0, R = chunks.size() - 1;
		while (L <= R)
		{
			long mid = (L + R) / 2;
			//DebugLog("EC: L=" + ToString(L) + " R=" + ToString(R)+" mid="+ToString(mid));
			if (chunks.at(mid).cx == cx && chunks.at(mid).cy == cy)
				return true;
			else if (cx > chunks.at(mid).cx || cx == chunks.at(mid).cx && cy > chunks.at(mid).cy)
				L = mid + 1;
			else
				R = mid - 1;
		}
		return false;
	}
	Chunk& FindChunk(const long& cx, const long& cy)
	{
		//DebugLog("FindChunk called");
		/*for (auto& chunk : chunks)
			if (chunk.cx == cx && chunk.cy == cy)
				return chunk;*/	//too slow
		long L=0, R=chunks.size()-1;
		while (L <= R)
		{
			long mid = (L + R) / 2;
			//DebugLog("EC: L=" + ToString(L) + " R=" + ToString(R) + " mid=" + ToString(mid));
			if (chunks.at(mid).cx == cx && chunks.at(mid).cy == cy)
				return chunks.at(mid);
			else if (cx > chunks.at(mid).cx || cx == chunks.at(mid).cx && cy > chunks.at(mid).cy)
				L = mid + 1;
			else
				R = mid - 1;
		}
		ErrorLogTip(NULL, "无法找到应该找到的区块：(" + ToString(cx) + "," + ToString(cy) + ")", "ChunkLoader::FindChunk");
		return chunks.at(0);
	}
	bool OuttaLoadedWorld(const long& x, const long& y) const
	{
		if (chunks.empty())	return true;
		for (const auto& chunk : chunks)
		{
			if (chunk.IsIn(x,y))
			{
				return false;
			}
		}
		return true;
	}
	Block& GetBlock(const long& x, const long& y, const char* tag="null")
	{
		if (OuttaLoadedWorld(x, y))
		{
			ErrorLogTip(NULL, "方块访问越界（区块未加载）: X=" + ToString(x) + " Y=" + ToString(y)+" tag="+ToString(tag), "ChunkLoader::GetBlock");
			return chunks.at(0).blocks.at(0);
		}
		long cx = ToChunkCoord(x), cy = ToChunkCoord(y);
		long rx = x % CHUNK_PX, ry = y % CHUNK_PX;
		return FindChunk(cx, cy).GetBlock(rx, ry, tag);
	}
};

class World {
	public:
		//vector<Block> blocks;
		ChunkLoader manager;
		
		long time;	//世界时间 
		
		vector<EntitySpawner> espawners;	//每个走过的群落一个 定期删除

		PIMAGE nearbyMap;
		vector<Item> insp_container;	//临时存放查看的容器内的物品 
		int insp_ctn_left, insp_ctn_top;	//屏幕坐标 
		int insp_w, insp_h;
#define insp_ctn_right (world.insp_ctn_left+world.insp_w*CONTAINER_SLOT_W)
#define insp_ctn_bottom (world.insp_ctn_top+world.insp_h*CONTAINER_ITEM_FS)
		short insp_precur;				//容器物品预索引 
		
		WP_ID wpid;
		vector<WeatherParticle> wps;
		clock_t lastMinTick;
		//clock_t lastWave;			//水波荡漾
		clock_t lastPlantShake;		//树的摇曳
		clock_t plantShakeCD;	//摇树冷却
		clock_t lastWPdrop;		//雨滴
		clock_t wpcd;				//雨滴冷却
		bool sunrisen;		//mark variable
		static const long Midnight = 0;		//0:00
		static const long Sunrise = 270;	//4:30
		static const long Morning = 330;	//5:30
		static const long Noon = 720;		//12:00
		static const long Sunset = 1110;	//18:30
		static const long Night = 1170;		//19:30

		World() {
			Init();
		}
		void Init() {
			if(!manager.chunks.empty())
				Clear();
			g_block_px=DEF_BLOCK_PX;
			nearbyMap = nullptr;
			lastMinTick = lastWPdrop = lastPlantShake = clock();
			time=0;
			insp_precur=-1;
			insp_ctn_left=insp_ctn_top=0;
			insp_w=insp_h=0;
			wpid = WPID_NONE;
			wpcd = WP_RAIN_DEF_CD;
			plantShakeCD = 10000;
			sunrisen = 0;
		}
		inline bool IsDay(void) const
		{
			return (time >= Morning && time < Night);
		}
		inline bool IsNight(void) const
		{
			return !IsDay();
		}
		void UpdateInspectedContainer(const Block& b);
		void SaveInspectedContainer(Block& b);
		short ContainerGainItem(ITEM_ID id, short cnt, const char* itag=nullptr);
		inline void UpdateTime(void);
		inline void GetTime(short& hour, short& minute) const
		{
			hour = time / 60;
			minute = time % 60;
		}
		inline void SetTime(short hour, short minute)
		{
			time = hour * 60 + minute;
		}
		string GetTime(void) const
		{
			short hour=0,minute=0;
			GetTime(hour,minute);
			char buf[6]{0};
			sprintf(buf, "%2d:%02d",hour,minute);
			return (string)buf;
		}
		LIGHT_LVL GetEnvironmentLight(void) const
		{
			if(time < Sunrise || time >= Night)
				return 1;
			else if(time >= Morning && time < Sunset)
				return NORMAL_LIGHT_LVL;
			else if(time >= Sunrise && time < Morning)
			{
				return 1 + ceil((time - Sunrise) / float(Morning - Sunrise) * (NORMAL_LIGHT_LVL - 1));
			}
			else if(time >= Sunset && time < Night)
			{
				return NORMAL_LIGHT_LVL - ceil((time - Sunset) / float(Night - Sunset) * (NORMAL_LIGHT_LVL - 1));
			}
			return 4;
		}
		void ShedArea(LIGHT_LVL base, long _left, long _top, long _right, long _bottom)
		{
			XCPT_L
			for(long h = 0; h < _bottom - _top + 1; ++h)
				for(long w = 0; w < _right - _left + 1; ++w)
				{
					if(OuttaWorld(_left + w, _top + h))	continue;
					GetBlock(_left+w,_top+h,"World::ShedArea").SetLight(base);
				}
			XCPT_R
		}
		void UpdateBurnerFuelValues(long _left, long _top, long _right, long _bottom)
		{
			XCPT_L
			static clock_t lastDec = clock();
			if(clock() - lastDec < 1000)	return;
			for(long h = 0; h < _bottom - _top + 1; ++h)
				for(long w = 0; w < _right - _left + 1; ++w)
				{
					if(OuttaWorld(_left + w, _top + h))	continue;
					GetBlock(_left+w,_top+h,"World::ShedArea").ReduceFuel(1);
				}
			lastDec = clock();
			XCPT_R
		}
		#define LF_K 5.0f
		inline LIGHT_LVL LightFormula(LIGHTNESS bright, float dist)	const
		{
//			if(fequ(bright, 0.0f))	return 0;
			auto res = (9 - dist / bright * LF_K);
			if(res < 0)	res = 0;
			return res;
		}
		bool PointLight(LIGHTNESS bright, long ox, long oy)
		{	//点光源照明算法 
			XCPT_L
			if(OuttaWorld(ox,oy) || fequ(bright, 0.0f))	return false;
			bool flag = false;
			DIR dir = LEFT;
			long x=ox,y=oy-1;
			short step = 0;
			short r = 1;	//radius
			GetBlock(ox,oy,"Worl::PointLight").SetLight(LightFormula(bright, 0.0f));
			do
			{
				flag = false;
				step = 0;
//				TrackLog("r="+ToString(r));
				while(step < 8 * r)
				{
					if(!OuttaWorld(x,y))
					{
						LIGHT_LVL rel = LightFormula(bright, Distance(x,y,ox,oy));
//						TrackLog(ToString(rel));
						bool res = GetBlock(x,y,"Worl::PointLight").AddLight(rel,false);
						if(res)	flag = true;
					}
					step++;
					DirOffsetPos(x,y,dir,"World::PointLight");
					if(dir == LEFT && step == 2*r-1)
						dir = DOWN;
					else if(dir == DOWN && step == 4*r-1)
						dir = RIGHT;
					else if(dir == RIGHT && step == 6*r-1)
						dir = UP;
				}
				++r;
				dir = LEFT;
			}while(flag);
			return true;
			XCPT_R
			return false;
		}
		void ApproachBlocksLightness(long _left, long _top, long _right, long _bottom)
		{	//处理方块亮度 
			XCPT_L
			for(long h = 0; h < _bottom - _top + 1; ++h)
				for(long w = 0; w < _right - _left + 1; ++w)
				{
					if(OuttaWorld(_left + w, _top + h))	continue;
					if(!blockdata.at(GetBlock(_left+w,_top+h,"World::ApproachBlocksLightness").id).IsLightSource())	continue;
					LIGHTNESS bright = GetBlock(_left+w,_top+h,"World::ApproachBlocksLightness").GetOwnLightness();
					if(!fequ(bright, 0.0f))
					{
						PointLight(bright, _left+w, _top+h);
					}
				}
			XCPT_R
		}
		void ApproachPlayerLightSource(void);
		void ApproachEntityLightSource(void);
		void UpdateSurroundingLight(void)
		{
			XCPT_L
			ShedArea(GetEnvironmentLight(),SURROUNDING_RANGE);	//基础全局亮度 plantShake
			ApproachBlocksLightness(SURROUNDING_RANGE);
			ApproachPlayerLightSource();
			ApproachEntityLightSource();
			XCPT_R
		}
		void UpdateSurroundingSounds(void);
#define PLANT_SHAKE_CD_MIN 2000
#define PLANT_SHAKE_CD_MAX 16000
		void UpdateEnvironmentFX(void)
		{
			if (clock() - lastPlantShake >= plantShakeCD)
			{
				plantShakeCD = RandomRange(PLANT_SHAKE_CD_MIN, PLANT_SHAKE_CD_MAX);
				lastPlantShake = clock();
			}
			if (wpid == WPID_RAINDROP && g_sndTriggers.at(g_sndmgr.GetSound("environment\\rain").index) != SNDTRG_REPEAT)
			{
				SNDLOOP("environment\\rain");
			}
			else if (wpid != WPID_RAINDROP && g_sndTriggers.at(g_sndmgr.GetSound("environment\\rain").index) == SNDTRG_REPEAT)
			{
				STOPSNDLOOP("environment\\rain");
			}
		}
		void UpdateEnvironmentChanges(void);
		void UpdateWeather()
		{
			if (wpid == WPID_RAINDROP || wpid == WPID_SNOWFLAKE)
			{
				if (clock() - lastWPdrop >= wpcd)
				{
					WeatherParticle wp;
					wp.id = wpid;
					wp.x = RandomRange(5, uix);
					wp.y = scene_top;
					wp.layer = Choice({ 0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,2,2 });
					if (wp.layer != 2)
						wp.end_y = RandomRange(100, scene_bottom);
					wps.push_back(wp);
					lastWPdrop = clock();
				}	
				for (size_t i = 0; i < wps.size(); ++i)
				{
					if (!wps.at(i).IsActive())
					{
						auto iter = wps.begin();
						size_t j = 0;
						while (iter != wps.end() && j != i) {
							iter++;
							j++;
						}
						iter = wps.erase(iter);
						continue;
					}
					wps.at(i).Update();
				}
				if (time % 500 == 50)
				{
					if (RandomRange(0, 100, true, false) < 3)
					{	//停止降水
						STOPSNDLOOP("environment\\rain");
						wpid = WPID_NONE;
						wps.clear();
					}
				}
			}
			else {
				if (time % 500 == 0)
				{
					if (RandomRange(0, 100, true, false) < 2)
					{	//开始降雨
						SNDLOOP("environment\\rain");
						wpid = WPID_RAINDROP;
						wpcd = WP_RAIN_DEF_CD;
						wps.clear();
					}
					else if (RandomRange(0, 1000, true, false) < 5)
					{	//开始降雪
						wpid = WPID_SNOWFLAKE;
						wpcd = WP_SNOW_DEF_CD;
						wps.clear();
					}
				}
			}
		}
#define RAINY_ALPHA_CHANGE_CD 10000
#define RAINY_ALPHA_CHANGE_TIME 3500
#define RAINY_ALPHA_BASE 215
#define RAINY_SINE_AMP 40
#define RAINY_SINE_AMP_2 10
#define RAINY_SINE_OMEGA (2*PI/RAINY_ALPHA_CHANGE_TIME)
#define RAINY_SINE_OMEGA_2 (1.6*PI/RAINY_ALPHA_CHANGE_TIME)
		unsigned char GetRainyAlpha() const
		{
			static clock_t lastChg = clock();
			if (clock() - lastChg >= RAINY_ALPHA_CHANGE_CD)
			{
				lastChg = clock();
			}
			else if (clock() - lastChg <= RAINY_ALPHA_CHANGE_TIME)
			{
				return RAINY_ALPHA_BASE + RAINY_SINE_AMP * sin(RAINY_SINE_OMEGA * (clock() - lastChg));
			}
			return RAINY_ALPHA_BASE + RAINY_SINE_AMP_2 * sin(RAINY_SINE_OMEGA_2 * (clock() - lastChg));
		}
#define SNOWY_SINE_CD 7000
#define SNOWY_SINE_TIME 3000
#define SNOWY_ALPHA_BASE 70
#define SNOWY_BLUR_BASE 160
#define SNOWY_SINE_OMEGA (2*PI/SNOWY_SINE_TIME)
#define SNOWY_SINE_OMEGA_2 (1.5*PI/SNOWY_SINE_TIME)
#define SNOWY_SINE_AMP 40
#define SNOWY_SINE_AMP_2 6
		unsigned char GetSnowyAlpha() const
		{
			static clock_t lastSine = clock();
			if (clock() - lastSine >= SNOWY_SINE_CD)
			{
				lastSine = clock();
			}
			else if (clock() - lastSine <= SNOWY_SINE_TIME)
			{
				return SNOWY_ALPHA_BASE + SNOWY_SINE_AMP * sin(SNOWY_SINE_OMEGA * (clock() - lastSine));
			}
			return SNOWY_ALPHA_BASE + SNOWY_SINE_AMP_2 * sin(SNOWY_SINE_OMEGA_2 * (clock() - lastSine));
		}
		unsigned char GetSnowyBlur() const
		{
			static clock_t lastSine = clock();
			if (clock() - lastSine >= SNOWY_SINE_CD)
			{
				lastSine = clock();
			}
			else if (clock() - lastSine <= SNOWY_SINE_TIME)
			{
				return SNOWY_BLUR_BASE + SNOWY_SINE_AMP * 2.0f * sin(SNOWY_SINE_OMEGA * (clock() - lastSine));
			}
			return SNOWY_BLUR_BASE + SNOWY_SINE_AMP_2 * 1.8f * sin(SNOWY_SINE_OMEGA_2 * (clock() - lastSine));
		}
		void Clear() {
			manager.SaveAndClear();
		}
		void DeleteNearbyMap(void)
		{
			if(!nearbyMap)	return;
			delimage(nearbyMap);
			nearbyMap=nullptr;
		}
		void UpdateNearbyMap(void) ;
		Block& GetBlock(long x, long y, const char* tag=nullptr) {
			XCPT_L

			if(x >= manager.map_w) {
				ErrorLogTip(NULL,"方块X坐标越界："+ToString(x)+">="+ToString(manager.map_w),"World::GetBlock"+(tag?ToString(tag):""));
				return manager.chunks.at(0).blocks.at(0);
			} else if(y >= manager.map_h) {
				ErrorLogTip(NULL,"方块Y坐标越界："+ToString(y)+">="+ToString(manager.map_h),"World::GetBlock"+(tag?ToString(tag):""));
				return manager.chunks.at(0).blocks.at(0);
			}
			return manager.GetBlock(x, y, tag);
			XCPT_R
		}
		bool SetBlock(long x, long y, BLOCK_ID id, BIOME_ID biome=biome_void, const char* tag="World::SetBlock", const char* btag=nullptr) {
			//biome_void当默认值
			XCPT_L
			Block& b = GetBlock(x,y,tag);
			JSON json;
			if(btag == nullptr) {
				//自动获取合适的JSON标签
				json = GetDefaultBlockJSON(id);
			} else {
				json = ParseJSON(btag,"World::SetBlock");
			}
			b.Reset(id,biome,json);
			return true;
			XCPT_R
		}
		void UpdateEntitySpawners();
} world;

PIMAGE PrintedMap(int px=1, long _left = 0, long _right = world.manager.map_w-1, long _top = 0, long _bottom = world.manager.map_h-1)
{
	if(_right <= _left || _bottom <= _top)	return nullptr;
//	DebugLog("left="+ToString(_left)+" right="+ToString(_right)+" top="+ToString(_top)+" bottom="+ToString(_bottom));
	PIMAGE img_map = newimage((_right-_left+1)*px,(_bottom-_top+1)*px);
	for(long y = _top; y <= _bottom; ++y)
		for(long x = _left; x <= _right; ++x)
		{
			if(OuttaWorld(x,y))	continue;
			for(short h = 0; h < px; h ++)
				for(short w = 0; w < px; w++)
				{
//					DebugLog("x="+ToString(x)+" y="+ToString(y)+" h="+ToString(h)+" w="+ToString(w));
					*(getbuffer(img_map)+((y-_top)*px+h)*(_right-_left+1)*px+(x-_left)*px+w) = world.GetBlock(x,y,"PrintedMap").color;
				}
		}
	return img_map;
}
void World::UpdateNearbyMap(void) 
{
	DeleteNearbyMap();
	nearbyMap = PrintedMap(NBMAP_PX,GetPlayerX() - NBMAP_W_HALF,GetPlayerX() + NBMAP_W_HALF,
									GetPlayerY() - NBMAP_H_HALF, GetPlayerY() + NBMAP_H_HALF);
}
void SaveMapToFile(const char* path)
{
	TrackLog("..");
	PIMAGE img_map = PrintedMap();
	saveimage(img_map, path);
	DebugLog("【保存】已将世界缩略地图保存至"+ToString(path));
}
inline color_t GetRaindropColor(void)
{
	color_t clr = WP_RAIN_COLOR;
	if (world.GetEnvironmentLight() <= 1)
		return EGERGB(1,1,1);
	return RateColor(clr, (world.GetEnvironmentLight() / (float)NORMAL_LIGHT_LVL));
}
inline color_t GetSnowflakeColor(void)
{
	color_t clr = WP_SNOW_COLOR;
	if (world.GetEnvironmentLight() <= 1)
		return EGERGB(5,5,5);
	return RateColor(clr, (world.GetEnvironmentLight() / (float)NORMAL_LIGHT_LVL));
}
void WeatherParticle::Draw() const
{
	if (!IsActive())	return;
	if (id == WPID_RAINDROP)
	{
		if (layer == 0)
			setfont(25, 0, "System");
		else if (layer == 1)
			setfont(50, 0, "System");
		else {
			setfont(350, 0, "System");
		}
		setcolor(GetRaindropColor());
		if (y < end_y - 50)
			xyprintf(x, y, "/");
		else if (y < end_y - 25) {
			xyprintf(x, y, "√");
		}
		else {
			xyprintf(x, y, "∴");
		}
	}
	else if (id == WPID_SNOWFLAKE)
	{
		if (layer == 0)
			setfont(25, 0, CHN_UNICODE_FONTNAME);
		else if (layer == 1)
			setfont(50, 0, CHN_UNICODE_FONTNAME);
		else {
			setfont(90, 0, CHN_UNICODE_FONTNAME);
		}
		setcolor(GetSnowflakeColor());
		if (layer == 0)
		{
			if (y < end_y - 50)
				xyprintf(x, y, L"\u2744");
			else if (y < end_y - 25) {
				xyprintf(x, y, L"\u273B");
			}
			else {
				xyprintf(x, y, L"\u273B");
			}
		}
		else if (layer == 1)
		{
			if (y < end_y - 50)
				pxprint_instant(x, y, 0x2744, 36);
			else if (y < end_y - 25) {
				pxprint_instant(x, y, 0x273B, 36);
			}
			else {
				pxprint_instant(x, y, 0x273B, 36);
			}
		}
		else {
			if (y < end_y - 50)
				pxprint_instant(x, y, 0x2744, 36);
			else if (y < end_y - 25) {
				pxprint_instant(x, y, 0x273B, 36);
			}
			else {
				pxprint_instant(x, y, 0x273B, 36);
			}
		}
	}
	
}
clock_t Entity::GetMoveCD(void) const
{
	if (OuttaWorld(x, y))	//Freeze
		return LONG_MAX;
	short move_cd;
	if (ai == AI_LINE)
	{
		bool suc = tag.GetSimpleValue<short>("MoveCD", &move_cd);
		if (!suc)
		{
			ErrorLogTip(NULL, "无法获取直线运动的实体移动冷却 en_name=" + entitydata.at(id).en_name, "Entity::GetMoveCD");
			return 1000;
		}
		return move_cd;
	}
	else if (ai == AI_CHASE || ai == AI_FLEE)
	{
		bool suc = entitydata.at(id).data.GetSimpleValue<short>("MoveCD", &move_cd);
		if (!suc)
		{
			ErrorLogTip(NULL, "无法获取直线运动的实体移动冷却 en_name=" + entitydata.at(id).en_name, "Entity::GetMoveCD");
			return 1000;
		}
		if (blockdata.at(world.GetBlock(x, y, "Entity::GetMoveCD").id).data.HaveKey("Viscidity")) {
			//具有粘性
			float vis = 1.0f;
			bool suc = blockdata.at(world.GetBlock(x, y, "Entity::GetMoveCD").id).data.GetSimpleValue<float>("Viscidity", &vis);
			if (!suc) {
				ErrorLogTip(NULL, "无法获取液体粘度\nX=" + ToString(x) + " Y=" + ToString(y) + " id=" + ToString(world.GetBlock(x, y, "Entity::GetMoveCD").id), "Entity::GetMoveCD");
				return move_cd * vis;
			}
			move_cd *= vis;
		}
		move_cd *= FXMoveCDCoefficient();
		return move_cd;
	}
	else if (ai == AI_IDLE)
	{	//另有计算
		return 0;
	}
	else if (ai == AI_STILL)
		return LONG_MAX;
	ErrorLogTip(NULL, "无效的AI类型=" + ToString(ai), "Entity::GetMoveCD");
	return 1000;
}
bool EntitySpawning::CanSpawnNow(void) const
{
	if (!TimeToSpawn())	return false;
	if (pdata == nullptr)	return false;
	if (pdata->timeLimit == 1 && world.IsDay())	return false;
	if (pdata->timeLimit == 2 && world.IsNight())	return false;
	return true;
}

BLOCK_ID ItemToBlock(ITEM_ID id);
class ItemData {
	public:
		ITEM_ID id;
		string cn_name;
		string en_name;

		string shape;
		color_t color;

		ITEM_TYPE type;
		short max_heap;

		JSON data;	//JSON

		ItemData() {
			id = 0;
			cn_name = "物品";
			en_name = "item";
			shape = "？";
			color = LIGHTGRAY;
			max_heap = 99;
			type = ITT_NULL;
		}
		inline bool IsLightSource(void) const
		{	//ItemDat a::
			return (data.HaveKey("Lightness") || type == ITT_BLOCK && blockdata.at(ItemToBlock(id)).IsLightSource());
		}
		inline bool IsFuel(void) const
		{
			return (data.HaveKey("Fuel") && data.GetValueType("Fuel") == "short");
		}
		short GetFuelValue(void) const
		{
			short fv;
			bool suc = data.GetSimpleValue<short>("Fuel",&fv);
			if(!suc) {
				ErrorLogTip(NULL,"无法获取物品燃值\nid="+ToString(id)+" en_name="+en_name,"ItemData::GetFuelValue");
				return 0;
			}
			return fv;
		}
		bool HaveTag(const string& s) const
		{
			if(!data.HaveKey("Tags"))	return false;
			vector<string> _tags;
			bool suc = data.GetComplexValue("Tags", &_tags);
			if(!suc)	return false;
			if(_tags.empty())	return false;
			for(short i = 0; i < _tags.size(); ++i)
			{
				if("string" != ::GetValueType(_tags.at(i)))	continue;
				if(UncoverValue(_tags.at(i),"string") == s)
		            return true;
			}
			return false;
		}
		LIGHTNESS GetOwnLightness() const
		{    //ItemData::
			if(data.HaveKey("Lightness"))
			{
				LIGHTNESS ln=0.0f;
				bool suc = data.GetSimpleValue<float>("Lightness",&ln);
				if(!suc)
				{
					ErrorLogTip(NULL,"获取物品发光亮度时遭到失败\nid="+ToString(id)+" en_name="+en_name,"ItemData::GetOwnLightness");
					return 0.0f;
				}
				return ln;
			}else if(type == ITT_BLOCK && blockdata.at(ItemToBlock(id)).IsLightSource())
			{
				return blockdata.at(ItemToBlock(id)).GetOwnLightness();
			}
			return 0.0f;
		}
		string GetComment(void) const
		{
			string ret;
			bool suc = data.GetSimpleValue<string>("Comment",&ret);
			if(!suc) {
				ErrorLogTip(NULL,"无法获取物品注解\nid="+ToString(id)+" en_name="+en_name,"ItemData::GetComment");
				return 0;
			}
			return ret;
		} 
		KBLEVEL GetKnockbackLevel(void)	const
		{
			if (!data.HaveKey("KnockbackLevel"))
				return 0;	//no kb
			short kb = 0;
			bool suc = data.GetSimpleValue<short>("KnockbackLevel", &kb);
			if (!suc)
			{
				ErrorLogTip(NULL, "无法获取物品击退力 en_name=" + en_name, "ItemData::GetKnockbackLevel");
				return 0;
			}
			return kb;
		}
		clock_t GetUseCD(void)	const {
			short cd=0;
			bool suc = data.GetSimpleValue<short>("CD",&cd);
			if(!suc) {
				ErrorLogTip(NULL,"无法获取物品使用冷却时间\nid="+ToString(id)+" en_name="+en_name,"ItemData::GetUseCD");
				return 0;
			}
			return (clock_t)cd;
		}
		short GetAttackDamage(void)	const {
			short at=0;
			bool suc = data.GetSimpleValue<short>("Attack",&at);
			if(!suc) {
				ErrorLogTip(NULL,"无法获取物品攻击伤害\nid="+ToString(id)+" en_name="+en_name,"ItemData::GetAttackDamage");
				return 0;
			}
			return at;
		}
		short GetFood(void)	const {
			short fd=0;
			bool suc = data.GetSimpleValue<short>("Food",&fd);
			if(!suc) {
				ErrorLogTip(NULL,"无法获取物品食物值\nid="+ToString(id)+" en_name="+en_name,"ItemData::GetFood");
				return 0;
			}
			return fd;
		}
		short GetProjectileMovingCD(void)	const {
			short pc=0;
			bool suc = data.GetSimpleValue<short>("ProjCD",&pc);
			if(!suc) {
				ErrorLogTip(NULL,"无法获取物品投射物飞行冷却\nid="+ToString(id)+" en_name="+en_name,"ItemData::GetProjectileMovingCD");
				return 0;
			}
			return pc;
		}
		short GetRange(void)	const {
			short rg=0;
			bool suc = data.GetSimpleValue<short>("Range",&rg);
			if(!suc) {
				ErrorLogTip(NULL,"无法获取物品射程\nid="+ToString(id)+" en_name="+en_name,"ItemData::GetRange");
				return 0;
			}
			return rg;
		}
		ENTITY_ID GetEntityID(void)	const {
			short eid=0;
			bool suc = data.GetSimpleValue<short>("EntityID",&eid);
			if(!suc) {
				ErrorLogTip(NULL,"无法获取物品对应实体ID\nid="+ToString(id)+" en_name="+en_name,"ItemData::GetEntityID");
				return 0;
			}
			return eid;
		}
		long GetEntityLife(void)	const {		//ms
			long el = 0;
			bool suc = data.GetSimpleValue<long>("EntityLife", &el);
			if (!suc) {
				ErrorLogTip(NULL, "无法获取物品对应实体存活期\nid=" + ToString(id) + " en_name=" + en_name, "ItemData::GetEntityLife");
				return 0;
			}
			return el;
		}
		short GetPower(void)	const {
			short pwr=0;
			bool suc = data.GetSimpleValue<short>("Power",&pwr);
			if(!suc) {
				ErrorLogTip(NULL,"无法获取物品Power\nid="+ToString(id)+" en_name="+en_name,"ItemData::GetPower");
				return 0;
			}
			return pwr;
		}
		short GetPowerLevel(void)	const {
			short pl=0;
			bool suc = data.GetSimpleValue<short>("PowerLevel",&pl);
			if(!suc) {
				ErrorLogTip(NULL,"无法获取物品PowerLevel\nid="+ToString(id)+" en_name="+en_name,"ItemData::GetPowerLevel");
				return 0;
			}
			return pl;
		}
		short GetDurability(void)	const {
			if (data.HaveKey("Unbreakable") && data.GetShort("Unbreakable") == 1)
				return SHRT_MAX;
			short dur=1;
			bool suc = data.GetSimpleValue<short>("Durability",&dur);
			if(!suc) {
				ErrorLogTip(NULL,"无法获取物品耐久度\nid="+ToString(id)+" en_name="+en_name,"ItemData::GetDurability");
				return 1;
			}
			return dur;
		}
};
vector<ItemData> itemdata;

ITEM_ID BlockToItem(BLOCK_ID bid) {
	for(ITEM_ID i = 0; i < itemdata.size(); ++i)
		if(itemdata.at(i).en_name == blockdata.at(bid).en_name)
			return i;
	ErrorLogTip(NULL,"无法根据方块ID获取对应物品ID","BlockToItem");
	return 0;
}
ITEM_ID CnNameToItem(const string& cn_name) {
	for(short i = 0; i < itemdata.size(); ++i)
		if(itemdata.at(i).cn_name == cn_name)
			return ITEM_ID(i);
	ErrorLogTip(NULL,"无法根据中文ID获取物品ID\ncn_name="+cn_name,"CnNameToItem");
	return 0;
}
ITEM_ID EnNameToItem(const string& en_name) {
	for(short i = 0; i < itemdata.size(); ++i)
		if(itemdata.at(i).en_name == en_name)
			return ITEM_ID(i);
	ErrorLogTip(NULL,"无法根据英文ID获取物品ID\ncn_name="+en_name,"EnNameToItem");
	return 0;
}

void ReadItemData() {
	if(!ExistFile(ITEMDATA_PATH)) {
		ErrorLogTip(NULL,"物品数据丢失","ReadItemData");
		return;
	}
	DebugLog("【读取】读取物品数据："+ITEMDATA_PATH);
	vector<string> lines = ReadFileLines(ITEMDATA_PATH);
	long i=1;
	while(i < lines.size()) {
		string line = lines.at(i);
		if(line.empty()) {
			++i;
			continue;
		}
		line = strrpc((char*)line.c_str(), " ", "@");	//加锁
		line = CSVResplitLine(line);
//		DebugLog(line);

		stringstream ss;
		ItemData idt;
		ss<<line;

		ss>>idt.id;
		ss>>idt.cn_name;
		ss>>idt.en_name;
		ss>>idt.shape;

		string tmp;
		ss>>tmp;
		idt.color = ParseColor(tmp);

		ss>>tmp;
		idt.type = ParseItemType(tmp);

		ss>>idt.max_heap;

		string tmptmp="";
		short c=0;
		while(!ss.eof()) {
			ss>>tmp;
			if(c!=0)	//2,3,4etc
				tmptmp+=",";
			tmptmp+=tmp;
			c++;
		}
		if(strhead(tmptmp)=="\"")
			tmptmp = strrpc((char*)strxhead(strxtail(tmptmp)).c_str(),"\"\"","\"");
		tmptmp = strrpc((char*)tmptmp.c_str(), "@", " ");	//还原
		idt.data = ParseJSON(tmptmp,"ReadItemData");
//		idt.data.ToLog();

		itemdata.push_back(idt);
		++i;
	}
}
class DamagePointData {		//伤害位点数据
public:
	DPID id;	//style id
	vector<POINT> dps;
	double oxratio;
	double oyratio;
	short xlen;
	short ylen;

	DamagePointData()
	{
		id = 0;
		oxratio = oyratio = 0.0;
		xlen = ylen = 0;
	}
	void Clear()
	{
		dps.clear();
	}
	bool Empty()
	{
		return dps.empty();
	}
};
vector <DamagePointData> dpdata{};

vector<POINT> GetDamagePoints(DPID style_id)
{	//获取的方向是 上 UP。
	if (dpdata.empty())
	{
		ErrorLogTip(NULL, "伤害位点数据还是空的:(", "GetDamagePoints");
		exit(-1);
	}
	return dpdata.at(style_id).dps;
}
#define SWAP_XY(pt) do{int t=pt.x;pt.x=pt.y;pt.y=t;}while(0)
void DamagePointsRotate(vector<POINT>& dps, DIR dir)
{	//旋转算法
	//UP -> 
	if (dir == RIGHT)
	{
		for (int i = 0; i < dps.size(); ++i)
		{
			SWAP_XY(dps.at(i));
			if (dps.at(i).x < 0)
				dps.at(i).x = -dps.at(i).x;
		}
	}
	else if (dir == DOWN)
	{
		for (int i = 0; i < dps.size(); ++i)
		{
			if (dps.at(i).y < 0)
				dps.at(i).y = -dps.at(i).y;
		}
	}
	else if (dir == LEFT)
	{
		for (int i = 0; i < dps.size(); ++i)
		{
			SWAP_XY(dps.at(i));
		}
	}//UP 不用管
	else {		//avoided
		//ErrorLogTip(NULL, "Invalid Dir!", "DamagePointsRotate");
		//exit(-1);
	}
}
vector<POINT> DamagePointsRotated(vector<POINT> oriDps, DIR dir)
{	//旋转算法
	DamagePointsRotate(oriDps, dir);
	return oriDps;
}
void ObtainOriginPosRatio(const DamagePointData& dpt, double& xratio, double& yratio, short& xlen, short& ylen)
{	//获取原点位置比 
	short xmin = 0, xmax = 0, ymin = 0, ymax = 0;
	//获取最值
	for (short i = 0; i < dpt.dps.size(); ++i)
	{
		if (dpt.dps.at(i).x < xmin)
			xmin = dpt.dps.at(i).x;
		else if (dpt.dps.at(i).x > xmax)
			xmax = dpt.dps.at(i).x;
		if (dpt.dps.at(i).y < ymin)
			ymin = dpt.dps.at(i).y;
		else if (dpt.dps.at(i).y > ymax)
			ymax = dpt.dps.at(i).y;
	}
	xlen = (xmax - xmin + 1);
	ylen = (ymax - ymin + 1);
	xratio = (0 - xmin) / float(xmax - xmin + 1);
	yratio = (0 - ymin) / float(ymax - ymin + 1);	//注意 ymin在ymax上面
}
inline void ObtainOriginPosRatio(DamagePointData& dpt)
{	//(single arg)
	ObtainOriginPosRatio(dpt, dpt.oxratio, dpt.oyratio, dpt.xlen, dpt.ylen);
}

#define DPFORMAT_MAX 1024
vector<POINT> AnalyseDPFormat(const string& s)
{
	//MSGPRINT(s);
	vector<string> data = CutLine(s);
	vector<POINT> dps{};
	stringstream ss;
	string tmp;
	ss << s;
	UINT p = 0;
	POINT tmppt;
	DWORD cnt = 0;
	while (1)
	{
		if (cnt >= DPFORMAT_MAX)
		{
			DebugLog("AnalyiseDPFormat: 数据过多，当场截断。");
			exit(-1);
			break;
		}
		ss >> tmp;
		//DebugLog((string)"tmp = " + tmp);
		if (tmp == "#")
		{
			p = 0;
			continue;		//begin -- no use
		}
		else if (tmp == "$")	break;	//end
		else {	//number
			cnt++;
			int i = atoi(tmp.c_str());
			//MSGPRINT(ToString(i));
			if (p == 0)
			{
				tmppt.x = i;
				p = 1;
			}
			else if (p == 1)
			{
				tmppt.y = i;
				p = 0;
				dps.push_back(tmppt);
				tmppt.x = 99;
				tmppt.y = 99;
			}
		}
	}
	return dps;
}
void ReadDPData()
{
	DebugLog((string)"读取伤害位点数据:" + DPDATA_PATH);
	if (!dpdata.empty())
		dpdata.clear();
	if (!ExistFile(DPDATA_PATH))
	{
		ErrorLogTip(NULL, (string)"找不到伤害位点数据于:" + DPDATA_PATH, "ReadDPData");
		return;
	}
	vector<string> lines = ReadFileLines(DPDATA_PATH);
	int i = 1;

	while (i < lines.size())
	{
		string line = lines.at(i);
		line = CSVResplitLine(line);
		//DebugLog(line, false);

		stringstream ss;
		ss << line;

		if (line.empty())
			break;

		DamagePointData dp;
		string no_use;
		string dpsFormat;

		ss >> dp.id;

		ss >> no_use;

		while (1)
		{
			string tmp;
			ss >> tmp;
			dpsFormat += tmp;
			if (tmp == "$")	break;
			dpsFormat += " ";
		}
		vector<POINT> dps = AnalyseDPFormat(dpsFormat);
		dp.dps = dps;
		ObtainOriginPosRatio(dp);

		dpdata.push_back(dp);
		i++;
	}
}
#define ITEM_SHAPE_RATIO 0.75
#define ITEM_CNT_RATIO 0.55
#define ET_MOVED_SHAKE_TIME 400
#define ET_MOVED_SHAKE_DISTANCE (0.13 * g_block_px)
void Entity::Draw() {
	//实体之绘画
	XCPT_L
	if(id == 0 || TimeToBeCleared()
	|| x < PL_VIEW_LEFT || x > PL_VIEW_RIGHT || y < PL_VIEW_TOP || y > PL_VIEW_BOTTOM	//出视野了 
	)
		return;	//不画 
	int scr_x=0;
	int scr_y=0;
	scr_x = GetPlayerDrawLeft() + (x - GetPlayerX()) * g_block_px;
	scr_y = GetPlayerDrawTop() + (y - GetPlayerY()) * g_block_px;
	short bpos=-1;
	if(GetBlockPos(&bpos))
	{	//微小位移 
		BlockPosOffset(scr_x,scr_y,bpos);
	}
	if (GetType() == ETT_MONSTER || GetType() == ETT_ANIMAL)
	{
		long x_offset = 0, y_offset = 0;
		DirOffsetPos(x_offset, y_offset, this->dir, "Entity::Draw");
		x_offset *= ET_MOVED_SHAKE_DISTANCE;
		y_offset *= ET_MOVED_SHAKE_DISTANCE;
		double _ratio = 1.0;
		if (clock() - lastMove < ET_MOVED_SHAKE_TIME)
		{
			_ratio = (clock() - lastMove) / double(ET_MOVED_SHAKE_TIME);
		}
		//_ratio = EaseInOutBack(_ratio);
		//scr_x = Lerp<int>(scr_x - x_offset, scr_x + x_offset, _ratio);
		//scr_y = Lerp<int>(scr_y - y_offset, scr_y + y_offset, _ratio);
		scr_x += x_offset * - sin(2 * PI*_ratio);
		scr_y += y_offset * - sin(2 * PI*_ratio);
	}
	if(GetType() == ETT_ITEM) {
		ITEM_ID iid=0;
		short cnt=1;
		bool suc = GetItemData(iid, cnt, nullptr);
		if(!suc || iid == 0 || cnt == 0)
			return;
//		DebugLog("item id="+ToString(iid)+" cnt="+ToString(cnt));
		setcolor(LightnessBlockColor(itemdata.at(iid).color, world.GetBlock(x,y,"Entity::Draw").light));
#ifndef ITEM_REAL_SHAPE
		setfont(g_block_px * ITEM_SHAPE_RATIO,0,"Determination Mono");
		pxprint(scr_x+g_block_px/2-g_block_px * ITEM_SHAPE_RATIO/2,
		        scr_y+g_block_px/2-g_block_px * ITEM_SHAPE_RATIO/2,
		        itemdata.at(iid).shape);
#else
		setfont(g_block_px * ITEM_SHAPE_RATIO,0,"楷体");
		if (itemdata.at(iid).shape.size() && itemdata.at(iid).shape[0] != 'U')
			xyprintf(scr_x + g_block_px / 2 - g_block_px * ITEM_SHAPE_RATIO / 2,
				scr_y + g_block_px / 2 - g_block_px * ITEM_SHAPE_RATIO / 2,
				itemdata.at(iid).shape.c_str());
		else {
			pxprint(scr_x + g_block_px / 2 - g_block_px * ITEM_SHAPE_RATIO / 2,
				scr_y + g_block_px / 2 - g_block_px * ITEM_SHAPE_RATIO / 2,
				HexToDec(strxhead(itemdata.at(iid).shape,1)));
		}
#endif
		if(cnt > 1) {
			setfont(g_block_px * ITEM_CNT_RATIO,0,"Determination Mono");
			setcolor(WHITE);
			pxprint(scr_x+g_block_px/2+2,
			        scr_y+g_block_px/2+g_block_px * ITEM_SHAPE_RATIO/2-10,
			        ToString(cnt));
		}
	} else if(IsProjectileET(GetType()))
	{	//proj
		size_t index=0;
		bool useRotation = false;
		if (entitydata.at(id).shapes.size() == 8)
			index = (dir - 1);
		else if (entitydata.at(id).shapes.size() == 4)
			index = (dir / 2 - 1);
		else if (entitydata.at(id).shapes.size() == 1)
		{
			index = 0;
			useRotation = true;
		}
		string shape = entitydata.at(id).shapes.at(index);
		color_t color = entitydata.at(id).colors.at(index);
		
//		setcolor(WHITE);
		if (useRotation)
		{
			PIMAGE img_proj = newimage(g_block_px, g_block_px);
			setcolor(LightnessBlockColor(color, world.GetBlock(x, y, "Entity::Draw").light));
			setfont(g_block_px, 0, "Determination Mono", img_proj);
			if (shape.size() && shape[0] != 'U')
				pxprint(0, 0, shape, 0, 0, img_proj);
			else {
				pxprint(0, 0,
					HexToDec(strxhead(shape, 1)), 0, 0, img_proj);
			}
			putimage_rotate(NULL, img_proj, scr_x + g_block_px*0.5, scr_y + g_block_px * 0.5, 0.5f, 0.5f, GetDirRadian(dir), 1, -1, 1);
			delimage(img_proj);
		}
		else {
			setfont(g_block_px, 0, "Determination Mono");
			setcolor(LightnessBlockColor(color, world.GetBlock(x, y, "Entity::Draw").light));
			if (shape.size() && shape[0] != 'U')
				pxprint(scr_x, scr_y, shape);
			else {
				pxprint(scr_x, scr_y,
					HexToDec(strxhead(shape, 1)));
			}
		}
		
	} else if(GetType() == ETT_MONSTER || GetType() == ETT_ANIMAL)
	{
		setfont(g_block_px,0,"Determination Mono");
		string shape = entitydata.at(id).shapes.at(GetShapeIndex());
		color_t color = entitydata.at(id).colors.at(GetColorIndex());
		if(IsBeingHurt())
			color = StepColor(color,LIGHTRED,0.6);
		color = LightnessBlockColor(color, world.GetBlock(x, y, "Entity::Draw").light);
		if (!IsAlive() && !TimeToBeCleared())
		{
			color = StepColor(color, BLACK, (clock() - lastDeath) / double(ET_DEATH_TIME));
		}
		setcolor(color);
		if (shape.size() && shape[0] != 'U')
			pxprint(scr_x, scr_y, shape);
		else {
			pxprint(scr_x, scr_y,
				HexToDec(strxhead(shape, 1)));
		}
		if (hp < GetMaxHp())
		{	//血条
			double _rate = hp / double(GetMaxHp());
			setfillcolor(world.IsDay()? BLACK : EGERGB(10,10,10) );
			bar(scr_x , scr_y + g_block_px - g_block_px*0.07f, scr_x + g_block_px, scr_y + g_block_px + g_block_px * 0.07f);
			setfillcolor(StepColor((color_t)RED, LIGHTGREEN, _rate));
			bar(scr_x + 2, scr_y + g_block_px- g_block_px * 0.04f, scr_x + 2 + _rate * (g_block_px - 4), scr_y + g_block_px+ g_block_px * 0.04f);
		}
	}
	else if (GetType() == ETT_BLADE)
	{
		double xratio = 0.0, yratio = 0.0;
		bool x_add = true;
		float radian = GetDirRadian(dir);
		DPID dpid = entitydata.at(id).GetDPID();
		PIMAGE img_blade = newimage(g_block_px*dpdata.at(dpid).xlen, g_block_px*dpdata.at(dpid).ylen);
		ObtainOriginPosRatio(dpdata.at(dpid), xratio, yratio, dpdata.at(dpid).xlen, dpdata.at(dpid).ylen);
		double centerx = (xratio * dpdata.at(dpid).xlen + 0.5) / double(dpdata.at(dpid).xlen);
		double centery = (yratio * dpdata.at(dpid).ylen + 0.5) / double(dpdata.at(dpid).ylen);	//新公式 

		auto dps = GetDamagePoints(dpid);
		size_t i = 0;
		for (const auto& dp : dpdata.at(dpid).dps)
		{
			if (entitydata.at(id).shapes.at(i).empty())	continue;
			int _x = 0, _y = 0;
			auto[rx, ry] = dp;
			_x = rx + dpdata.at(dpid).xlen * centerx;
			_y = ry + dpdata.at(dpid).ylen * centery;
			setfont(g_block_px, 0, "Determination Mono");
			setcolor( LightnessBlockColor(entitydata.at(id).colors.at(i),
				          world.GetBlock(x+rx,y+ry,"Entity::Draw").light));

			if(entitydata.at(id).shapes.at(i).size() && strhead(entitydata.at(id).shapes.at(i)) != "U")
				pxprint(_x*g_block_px, _y*g_block_px, entitydata.at(id).shapes.at(i),0,0, img_blade);
			else
				pxprint(_x*g_block_px, _y*g_block_px, HexToDec(strxhead(entitydata.at(id).shapes.at(i))), 0, 0, img_blade);
			++i;
		}

		//dps = DamagePointsRotated(dps, dir);		//rotation
		//if (!fequ(centerx, 0.5))
		//	x_add = false;
		//if (x_add)
		//	scr_x += g_block_px * .5;
		scr_x += g_block_px * .5;
		if (dir == RIGHT || dir == UP)
			scr_y += g_block_px * .5;
		//<!> 此处特判 微调
		if (dir == LEFT)
			scr_y += g_block_px * 0.3;
		if (dir == RIGHT)
			scr_x -= g_block_px * 0.3;
		if (dir == DOWN)
			scr_x -= g_block_px * 0.3, scr_y += g_block_px * 0.3;
		//putimage_withalpha(NULL, img_blade, scr_x, scr_y);
		putimage_rotate(NULL, img_blade, scr_x, scr_y, centerx, centery, radian, 1, -1, 1);
		delimage(img_blade);
	}
	else {
		ErrorLogTip(NULL,"无效的实体类型="+ToString(GetType()),"Entity::Draw");
		setcolor(entitydata.at(id).colors.at(GetColorIndex()));
		if (entitydata.at(id).shapes.at(GetShapeIndex()).size() && entitydata.at(id).shapes.at(GetShapeIndex())[0] != 'U')
			pxprint(scr_x,scr_y,entitydata.at(id).shapes.at(GetShapeIndex()));
		else
			pxprint(scr_x, scr_y, HexToDec(strxhead(entitydata.at(id).shapes.at(GetShapeIndex()))));
	}
	XCPT_R
}
inline bool HaveDamageItemType(ITEM_TYPE type) {
	return (type == ITT_CLOSE_WEAPON || type == ITT_GUN || type == ITT_BOW
	        || type == ITT_CROSSBOW || type == ITT_RPG || type == ITT_LAUNCHER
	        || type == ITT_PICKAXE || type == ITT_AXE || type == ITT_THROWABLE);
}
inline bool HaveCDItemType(ITEM_TYPE type) {
	return (type == ITT_PICKAXE || type == ITT_CLOSE_WEAPON || type == ITT_AXE
		|| type == ITT_GUN || type == ITT_BOW || type == ITT_CROSSBOW
		|| type == ITT_LAUNCHER || type == ITT_RPG || type == ITT_THROWABLE);
}
inline bool HaveDurabilityItemType(ITEM_TYPE type) {
	return (type == ITT_PICKAXE || type == ITT_CLOSE_WEAPON || type == ITT_AXE
		|| type == ITT_GUN || type == ITT_BOW || type == ITT_CROSSBOW
		|| type == ITT_LAUNCHER || type == ITT_RPG);
}
inline bool LongWeaponItemType(ITEM_TYPE type) {
	return (type == ITT_GUN || type == ITT_BOW || type == ITT_CROSSBOW
	        || type == ITT_RPG || type == ITT_LAUNCHER);
}
struct ItemDefaultJSONData {
	ITEM_ID id;
	JSON json;

	ItemDefaultJSONData() {
		id=0;
	}
};
vector<ItemDefaultJSONData> idjsondata;
void ReadItemDefaultJSONData() {
	if(!ExistFile(IDJSONDATA_PATH)) {
		ErrorLogTip(NULL,"物品默认JSON数据丢失","ReadItemDefaultJSONData");
		return;
	}
	DebugLog("【读取】读取物品默认JSON数据："+IDJSONDATA_PATH);
	vector<string> lines = ReadFileLines(IDJSONDATA_PATH);
	long i=1;
	while(i < lines.size()) {
		ItemDefaultJSONData idjd;
		stringstream ss;
		ss<<lines.at(i);

		ss>>idjd.id;

		string tmp,tmptmp;
		short c=0;
		ss>>tmp;
		while(!ss.eof()) {
			ss>>tmp;
			if(c!=0)
				tmptmp+=" ";
			tmptmp+=tmp;
			c++;
		}
		idjd.json = ParseJSON(tmptmp,"ReadItemDefaultJSONData");
		idjsondata.push_back(idjd);
		++i;
	}
}

JSON GetDefaultItemJSON(ITEM_ID id) {
	for(short j = 0; j < idjsondata.size(); ++j)
		if(idjsondata.at(j).id == id)
			return idjsondata.at(j).json;
//	ErrorLogTip(NULL,"无法根据物品ID获取默认JSON数据","GetDefaultItemJSON");
	return JSON();
}
string GetItemInitialTag(ITEM_ID id)
{
	JSON json = GetDefaultItemJSON(id);
	if(itemdata.at(id).data.HaveKey("Durability"))
	{
		json.AddItem("Durability",ToString(itemdata.at(id).GetDurability()));
	}
	return json.ToString();
}

#define HOTBAR_ITEM_CNT 10
#define ITEM_CNT 40
#define ITEM_DEF_FS 55
int g_item_fs = ITEM_DEF_FS;
#define ITEM_FS g_item_fs
#define CONTAINER_ITEM_FS (ITEM_FS*0.6f)
#define SWAP_CD 400
class Item {	//物品
	public:
		ITEM_ID id;
		short cnt;
		JSON tag;

		Item() {
			Init();
		}
		Item(ITEM_ID _id, short _cnt, const char* _tag = "{}")
		{
			id = _id, cnt = _cnt, tag = ParseJSON(_tag, "Item::Item");
		}
		void Init() {
			id = 0;
			cnt = 0;
		}
		void Assign(const Item& src)
		{
			id = src.id;
			cnt = src.cnt;
			tag = src.tag;
		}
		inline bool IsLightSource(void) const
		{
			return itemdata.at(id).IsLightSource();
		}
		inline bool IsFuel(void) const
		{
			return itemdata.at(id).IsFuel();
		}
		inline short GetFuelValue(void) const
		{
			return itemdata.at(id).GetFuelValue();
		}
		inline LIGHTNESS GetOwnLightness(void) const
		{
			return itemdata.at(id).GetOwnLightness();
		}
		inline bool HaveTag(const string& s) const
		{
			return itemdata.at(id).HaveTag(s);
		}
		JSON GetItemBlockTag(void) const
		{
			if(!tag.HaveKey("BlockTag") || tag.GetValueType("BlockTag") != "json")	return JSON();
			JSON btag;
			bool suc = tag.GetComplexValue("BlockTag",&btag);
			if(!suc)
			{
				ErrorLogTip(NULL,"无法获取物品方块数据标签,id="+ToString(id),"Item::GetItemBlockTag");
				return JSON();
			}
			return btag;
		}
		void Reset(ITEM_ID _id, short _cnt, const char* _tag=nullptr) {
			id = _id;
			cnt = _cnt;
			if(_tag)
				tag = ParseJSON(_tag,"Item::Reset");
			else
				AdaptDurability();
		}
		void Reset(ITEM_ID _id, short _cnt, const string& _tag) {
			id = _id;
			cnt = _cnt;
			if(_tag!="")
				tag = ParseJSON(_tag,"Item::Reset");
			else
				AdaptDurability();
		}
		void Reset(ITEM_ID _id, short _cnt, const JSON& _json) {
			id = _id;
			cnt = _cnt;
			if(!_json.Empty())
				tag = _json;
			else
				AdaptDurability();
		}
		inline bool Unbreakable(void) const
		{
			return (itemdata.at(id).data.HaveKey("Unbreakable") 
				             && itemdata.at(id).data.GetShort("Unbreakable") == 1
				 || tag.HaveKey("Unbreakable") 
				             && tag.GetShort("Unbreakable") == 1);
		}
		void AdaptDurability(void) {
			if (Unbreakable())	return;
			if(HaveDurabilityItemType(GetType())) {
				if(tag.HaveKey("Durability")) {
					tag.SetShort("Durability",itemdata.at(id).GetDurability());
				} else {
					tag.AddItem("Durability",ToString(itemdata.at(id).GetDurability())+"s");
				}
			}
		}
		bool Damage(short decr = 1);
		void Clear() {
			id = 0;
			cnt = 0;
			tag.Clear();
		}
		inline ITEM_TYPE GetType(void)	const {
			return itemdata.at(id).type;
		}
		short GetDur(void)	const {
			if (Unbreakable())	return SHRT_MAX;
			short dur=1;
			bool suc = tag.GetSimpleValue("Durability",&dur);
			if(!suc) {
				ErrorLogTip(NULL,"无法获取耐久度\nid="+ToString(id),"Item::GetDur");
				return dur;
			}
			return dur;
		}
		void Normalize() {
			if(cnt < 1 && id != 0)
				Clear();
			if (!Unbreakable() && HaveDurabilityItemType(GetType()) && GetDur() <= 0)
			{
				SND("break");
				Clear();	//报废
			}
			if(cnt > GetMaxHeap())	//超最大堆叠 
				   cnt = GetMaxHeap();
            if(!Unbreakable() && HaveDurabilityItemType(GetType()) && GetDur() > itemdata.at(id).GetDurability())	//超上限 
			    Damage(GetDur() - itemdata.at(id).GetDurability());
            if(GetType()==ITT_BLOCK && tag.HaveKey("BlockTag"))
            {	//空标签就删掉 
            	JSON btag;
            	bool suc = tag.GetComplexValue("BlockTag",&btag);
            	if(suc && btag.Empty())
            	    tag.RemoveItem("BlockTag");
			}
		}
		inline bool IsEmptyTag(void) const {
			return tag.Empty();
		}
		inline short GetMaxHeap(void)	const {
			return itemdata.at(id).max_heap;
		}
		inline bool IsFull(void)	const {
			return cnt >= GetMaxHeap();
		}
		vector<string> GetDescription(void)	const {
			vector<string> ret;
			if(id == 0)
				return ret;
			stringstream ss;
			ss<<itemdata.at(id).cn_name;
			if(cnt > 1)
				ss<<" ("<<cnt<<")";
			ss<<endl;
			ITEM_TYPE type = GetType();
			if(HaveDamageItemType(type)) {
				ss << "[攻击] " << itemdata.at(id).GetAttackDamage() << " 点\n";
				ss << "[冷却] " << itemdata.at(id).GetUseCD() << " ms\n";
				ss << "[击退力] " << itemdata.at(id).GetKnockbackLevel() << " 点\n";
			}
			if(type == ITT_AXE) {
				ss<<"[砍伐力] "<<itemdata.at(id).GetPower()<<"\n";
				ss<<"[砍伐等级] "<<itemdata.at(id).GetPowerLevel()<<"\n";
			} else if(type == ITT_PICKAXE) {
				ss<<"[挖掘力] "<<itemdata.at(id).GetPower()<<"\n";
				ss<<"[挖掘等级] "<<itemdata.at(id).GetPowerLevel()<<"\n";
			} else if(LongWeaponItemType(type)) {
				ss<<"[射程] "<<itemdata.at(id).GetRange()<<" 格\n";
			}
			if (itemdata.at(id).data.HaveKey("MagicCost"))
			{
				ss << "[魔法消耗] " << itemdata.at(id).data.GetShort("MagicCost") << " 点\n";
			}
			if(HaveDurabilityItemType(type)) {
				if (Unbreakable())
					ss << "无法被破坏"<<endl;
				else
					ss<<"[耐久] "<<GetDur()<<" / "<<itemdata.at(id).GetDurability()<<endl;
			}

			if (type == ITT_BLOCK)
				ss << "方块\n";
			else if (type == ITT_MATERIAL)
				ss << "材料\n";
			else if (type == ITT_CLOSE_WEAPON)
				ss << "近战武器\n";
			else if (type == ITT_AXE)
				ss << "斧子类\n";
			else if (type == ITT_PICKAXE)
				ss << "镐子类\n";
			else if (type == ITT_GUN)
				ss << "消耗子弹\n";
			else if (type == ITT_BOW || type == ITT_CROSSBOW)
				ss << "消耗箭矢\n";
			else if (type == ITT_FOOD)
				ss << "食品\n";
			else if (type == ITT_THROWABLE)
				ss << "投掷物\n";
			else if(type == ITT_RPG)
				ss << "消耗火箭弹\n";
			else if(type == ITT_LAUNCHER)
				ss << (itemdata.at(id).data.HaveKey("MagicCost")?"魔法武器\n":"发射器\n");
			else if(type == ITT_ARROW)
				ss << "箭矢类\n";
			else if(type == ITT_BULLET)
				ss << "枪弹类\n"; 
			else if(type == ITT_ROCKET)
				ss << "火箭弹类\n"; 
			else if(type == ITT_OTHER_PROJECTILE)
				ss << "弹药\n"; 
			else
				ss << "其他物品\n";
			string cmt;
			if(itemdata.at(id).data.HaveKey("Comment") && (cmt = itemdata.at(id).GetComment()) != "")
				ss<<"* "<<cmt<<'\n';
			ss<<itemdata.at(id).en_name;
			ret = CutLine(ss.str(),'\n');
			return ret;
		}
		void SwapWith(Item& theOther, clock_t& lastSwap, bool considerCD=true) {
			if(considerCD && clock() - lastSwap < SWAP_CD)
				return;
			lastSwap = clock();
			Normalize();
			theOther.Normalize();
			Item tmp;
			tmp.Reset(this->id,this->cnt,this->tag.ToString().c_str());
			this->Reset(theOther.id,theOther.cnt,theOther.tag.ToString().c_str());
			theOther.Reset(tmp.id,tmp.cnt,tmp.tag.ToString().c_str());
		}
		void Lose(short cnt) {
			//失去物品
			this->cnt -= cnt;
			if(this->cnt <= 0)
				this->Clear();
		}
		bool Gain(short cnt, short* overflow_cnt) {
			//获得物品   若溢出则返回false overflow_cnt接住溢出物品数量
			this->cnt += cnt;
			if(this->cnt > GetMaxHeap()) {
				if(overflow_cnt) {
					*overflow_cnt = this->cnt - GetMaxHeap();
					this->cnt = GetMaxHeap();
				} else {
					ErrorLogTip(NULL,"物品溢出且无指针救急\n"+ToString(this->cnt)+" / "+ToString(GetMaxHeap()),"Item::Gain");
				}
				return false;
			}
			return true;
		}
		void RandomItem(void) {
			id = RandomRange(1,itemdata.size()-1);
			cnt = GetMaxHeap();
		}
#define CM_NORMAL 0
#define CM_CURRENT 1
#define CM_PRECURRENT 2
#define CM_TMPITEM 3

#define CMSC_NORMAL_RATE 0.58f
#define CMSC_PRECUR_RATE 0.75f
#define SLOT_W (ITEM_FS*3)
#define CONTAINER_SLOT_W (CONTAINER_ITEM_FS*3)
		void Draw(long x, long y, BYTE curMode, bool isSimple, color_t bgorig=EGERGB(25,25,25), bool small=false) {
			//Item::Dra w
			Normalize();
			int fs = small?CONTAINER_ITEM_FS:ITEM_FS;
			int sw = small?CONTAINER_SLOT_W:SLOT_W;
			setfont(fs,0,"Determination Mono");
			if(curMode == CM_CURRENT) {
				if(!isSimple) {
					setfillcolor(bgorig);
					bar(x,y,x+fs*6,y+fs);	//gray rectangle
				} else {
					setfillcolor(bgorig);
					bar(x,y,x+sw*0.8,y+fs);	//gray rectangle
				}
				setcolor(YELLOW);
				pxprint(x-fs,y,"》");
				if(!isSimple)
					pxprint(x+(fs)*5,y,"《");
			} else if(curMode == CM_PRECURRENT) {
				if(isSimple) {
					setfillcolor(RateColor(bgorig,small?1.25f:1.0f));
					bar(x,y,x+sw*0.8,y+fs);	//gray rectangle
				} else {
					setcolor(DARKGRAY);
					pxprint(x-fs,y,"〉");
					pxprint(x+(fs)*5,y,"〈");
				}
			}
			if(id == 0)
				return;
			if(curMode == CM_NORMAL)
				setcolor(RateColor(itemdata.at(id).color,CMSC_NORMAL_RATE));
			else if(curMode == CM_PRECURRENT)
				setcolor(RateColor(itemdata.at(id).color,CMSC_PRECUR_RATE));
			else	//CM_CURRENT or CM_TMPITEM
				setcolor(itemdata.at(id).color);
			if (itemdata.at(id).shape.size()>0 && itemdata.at(id).shape[0] != 'U')
				pxprint(x,y, itemdata.at(id).shape);
			else {
				pxprint(x,y,
					HexToDec(strxhead(itemdata.at(id).shape, 1)));
			}
			if (HaveDurabilityItemType(GetType()) && !Unbreakable())
			{
				double _ratio = (double)GetDur() / itemdata.at(id).GetDurability();
				if (options.durability_bar)
				{		//耐久条
					if (!fequ(_ratio, 1.0))
					{
						setfillcolor(StepColor((color_t)RED, (color_t)LIGHTGREEN, _ratio));
						bar(x, y + fs - fs * 0.07, x + fs * _ratio, y + fs);
					}
				}
				else {	//百分比
					setcolor(BLACK);
					setfont(fs*0.47, 0, "Courier");
					xyprintf(x + 2, y + fs - fs * 0.47+1, "%2.0f%%", _ratio*100.0);
					setwcolor(StepColor((color_t)RED, (color_t)LIGHTGREEN, _ratio));
					xyprintf(x + 1, y + fs - fs * 0.47, "%2.0f%%", _ratio*100.0);
				}
			}
			setfont(fs, 0, "Determination Mono");
			if(!isSimple) {
				setcolor(LIGHTGRAY);
				setfont((fs) * 0.6,0,"Determination Mono");
				pxprint(x+(fs)*2,y," × ");
				setfont(fs,0,"Determination Mono");
				if(IsFull()) {
					if(curMode == CM_CURRENT)
						setcolor(LIGHTGREEN);
					else if(curMode == CM_PRECURRENT )
						setcolor(LIGHTCYAN);
					else
						setcolor(YELLOW);
					pxprint(x+(fs)*3.5,y,ToString(cnt));
				} else {
					if(curMode == CM_CURRENT)
						setcolor(LIGHTGRAY);
					else if(curMode == CM_PRECURRENT)
						setcolor(MAGENTA);
					else
						setcolor(DARKGRAY);
					//				setcolor(EgeColor(StepColor(WinColor(DARKGRAY),WinColor(isSelected?LIGHTGREEN:YELLOW), (cnt / (double)GetMaxHeap()))));
					pxprint(x+(fs)*3.5,y,ToString(cnt));
				}
			} else {
				setcolor(LIGHTGRAY);
				pxprint(x+fs, y, ToString(cnt));
			}

		}
};

ITEM_TYPE GetLauncherProjectileType(ITEM_TYPE lautype)
{
	if(lautype == ITT_BOW)
		return ITT_ARROW;
	if(lautype == ITT_GUN)
		return ITT_BULLET;
	if(lautype == ITT_RPG)
		return ITT_ROCKET;
	if(lautype == ITT_LAUNCHER)
		return ITT_OTHER_PROJECTILE;
	return ITT_NULL;
}
Item ParseItem(const JSON& json);
Item Block::GetFruit(void) const
{
	Item ret;
	ret.id = ret.cnt = 0;
	if(!blockdata.at(id).data.HaveKey("Fruit"))	return ret;
	//e.g.  {Fruit:"159s*1s*{Enchanted:1s}"}
	string fruitstr="";
	BOOL bret = blockdata.at(id).data.GetSimpleValue<string>("Fruit",&fruitstr);
	if(!bret)	return ret;
	if(fruitstr.empty())	return ret;
	//DebugLog("fruitstr=" + fruitstr);
	vector<string> cut = CutLine(fruitstr,'*');
	ret.cnt=1;
	ret.id = Convert<string,short>(UncoverValue(cut[0],"short"));
	//DebugLog("ret.id=" + ToString(int(ret.id)));
	if(cut.size()>1)
	{
		ret.cnt = Convert<string,short>(UncoverValue(cut[1],"short"));
	}
	if(cut.size()>2)
	{	//itag
		ret.tag = ParseJSON(cut[2],"Block::GetFruit");
	}
	TrackLog("GetFruit: id="+ToString(ret.id)+" cnt="+ToString(ret.cnt)+" itag="+ret.tag.ToString());
	return ret;
}
Item Block::PickFruit(void)
{
	Item item; item.id=item.cnt=0;
	if(!HaveFruit())	return item;
	Item fruit = GetFruit();
	//DebugLog("fruit id=" + ToString((int)fruit.id) + " cnt=" + ToString(fruit.cnt));
	tag.RemoveItem("Fruit");	//移除 
	return fruit;
}
void Block::DropFruit(const long& x,const long& y)
{
	if(!HaveFruit())	return;
	Item fruit = GetFruit();
	SpawnItem(x,y,fruit.id,fruit.cnt,fruit.tag.ToString().c_str());
}
Item& Block::GetContainerItem(short index) const 
{
	XCPT_L
	if(world.insp_container.size() <= index)
	{ 
		ErrorLogTip(NULL,"容器物品访问索引越界: "+ToString(index)+" >= "+ToString(world.insp_container.size()),"Block::GetContainerItem");
		return world.insp_container.at(0);
//		world.UpdateInspectedContainer(*this);
	}
	return world.insp_container.at(index);
	XCPT_R
}
void World::UpdateInspectedContainer(const Block& b)
{
	XCPT_L
	TrackLog("1");
	insp_container.clear();
	if(!b.HaveClass(BCLASS_CONTAINER))	return;
	short _w=blockdata.at(b.id).GetContainerWidth();
	short _h=blockdata.at(b.id).GetContainerHeight();
	for(short i = 0; i < _w*_h; ++i)
	{	//先装满空物品 
		Item item;
		insp_container.push_back(item);
	}
	//DebugLog("insp_container.size=" + ToString(insp_container.size()));
	if(!b.tag.HaveKey("Content") || b.tag.GetValueType("Content") != "list")	return;
	vector<string> raws;
	bool suc = b.tag.GetComplexValue("Content",&raws);
	if(!suc)
	{
		ErrorLogTip(NULL,"无法读取容器方块内含物","World::UpdateInspectedContainer");
		return;
	}
//	DebugLog("Read:"+ToString(raws));
	TrackLog("size="+ToString(raws.size()));
	if(raws.empty())	return;
	for(short j = 0; j < raws.size(); ++j)
	{
		TrackLog(raws.at(j));
		JSON json = ParseJSON(raws.at(j),"World::UpdateInspectedContainer");
		if(json.HaveKey("Slot"))
		{
			short slot = -1;
			bool suc = json.GetSimpleValue<short>("Slot",&slot);
			if(!suc)
			{
				ErrorLogTip(NULL,"无法获取容器方块内某物品的索引","World::UpdateInspectedContainer");
				continue;
			}
			insp_container.at(slot) = ParseItem(json);	//change the item
		}
	}
	TrackLog("end");
	XCPT_R
}
void SaveContainer(Block& b, const vector<Item>& buffer)
{
	XCPT_L
	vector<string> content;
	for (short j = 0; j < buffer.size(); ++j)
	{
		if (buffer.at(j).id != 0)
		{
			string s;
			s += "{Slot:";
			s += ToString(j);
			s += "s,Item:";
			s += ToString(buffer.at(j).id);
			s += "s";
			if (buffer.at(j).cnt > 1)
			{
				s += ",Count:";
				s += ToString(buffer.at(j).cnt);
				s += "s";
			}
			if (!buffer.at(j).tag.Empty())
			{
				s += ",tag:";
				s += buffer.at(j).tag.ToString();
			}
			s += "}";
			//			DebugLog(s);
			content.push_back(s);
		}
	}
	TrackLog("1");
	if (b.tag.HaveKey("Content"))
		b.tag.SetList("Content", content);
	else {
		b.tag.AddItem("Content", ToString(content));
	}
	//	DebugLog("Save:"+b.tag.ToString());
	XCPT_R
}
void World::SaveInspectedContainer(Block& b)
{	//保存容器物品至方块数据 
	SaveContainer(b, insp_container);
}
short World::ContainerGainItem(ITEM_ID id, short cnt, const char* itag/*=nullptr*/)
{
	XCPT_L
	//返回剩余个数 成功则返回0
	for (short i = 0; i < insp_container.size(); ++i) {
		if (insp_container.at(i).id == id
			//					&& (itag==nullptr || (!container.at(i).tag.Empty() && container.at(i).tag.ToString() == string(itag)))
			&& insp_container.at(i).cnt < insp_container.at(i).GetMaxHeap()) {
			short original_cnt = insp_container.at(i).cnt;
			short of_cnt = 0;
			bool of = insp_container.at(i).Gain(cnt, &of_cnt);
			if (of) {
				cnt -= insp_container.at(i).GetMaxHeap() - original_cnt;
			}
			if (cnt <= 0) {
				cnt = 0;
				break;
			}
		}
	}
	for(short i = 0; i < insp_container.size(); ++i) {
		 if(insp_container.at(i).id == 0) {
			//空位
			insp_container.at(i).id = id;
			insp_container.at(i).cnt = cnt;
			if(itag)
				insp_container.at(i).tag = ParseJSON(itag,"World::ContainerGainItem");
			else
				insp_container.at(i).AdaptDurability();

			if(insp_container.at(i).cnt > insp_container.at(i).GetMaxHeap()) {
				//溢出
				cnt = (insp_container.at(i).cnt - insp_container.at(i).GetMaxHeap());
				insp_container.at(i).cnt = insp_container.at(i).GetMaxHeap();
			} else {
				cnt = 0;
				break;
			}
		}
	}
	return cnt;
	XCPT_R
}
void DropContentItems(long x, long y, const vector<string>& content)
{
	XCPT_L
	if(content.empty())	return;
	for(short j = 0; j < content.size(); ++j)
	{
		Item item;
		item = ParseItem(ParseJSON(content.at(j),"DropContentItems"));
		SpawnItem(x,y, item.id, item.cnt, item.tag.ToString().c_str());
	}
	XCPT_R 
} 
#define BAG_OPEN_CD 350

#define rui_left (scene_right + 15)
#define rui_right (uix - 5)
#define rui_text_top (20*(uiy / 1080.0))
#define inven_top (uiy*0.42)
void AdaptItemFontSize(void) {
	if(inven_top + ITEM_FS * (HOTBAR_ITEM_CNT + 1) > uiy) {	//下面超出屏幕
		ITEM_FS = (uiy - inven_top) / (HOTBAR_ITEM_CNT + 1);
	} else {
		ITEM_FS = ITEM_DEF_FS;
	}
}

Item ParseItem(const JSON& json)
{
	XCPT_L 
	Item ret;
	if(json.HaveKey("Item"))
	{
		short iid=0;
		bool suc = json.GetSimpleValue<short>("Item",&iid);
		if(!suc)
		{
			ErrorLogTip(NULL,"无法获取物品ID","ParseItem");
			return ret;
		}
		ret.id = iid;
	}
	if(json.HaveKey("Count"))
	{
		bool suc = json.GetSimpleValue<short>("Count",&ret.cnt);
		if(!suc)
		{
			ErrorLogTip(NULL,"无法获取物品数量","ParseItem");
			return ret;
		}
	}else
		ret.cnt = 1;
	if(json.HaveKey("tag"))
	{
		bool suc = json.GetComplexValue("tag",&ret.tag);
		if(!suc)
		{
			ErrorLogTip(NULL,"无法获取物品数据标签","ParseItem");
			return ret;
		}
	}
	return ret;
	XCPT_R
}
class Inventory {
	public:
		Item items[ITEM_CNT];
		Item tmpItem;
		short tmpItemFrom;
		bool isOpen;
		short cur;
		short precur;

		clock_t lastOpen;
		clock_t lastSwap;

		Inventory() {
			Init();
		}
		void Init() {
			cur=0;
			precur = -1;
			for(short i = 0; i < ITEM_CNT; ++i)
				items[i].Init();
			tmpItem.Init();
			tmpItemFrom = -1;
			isOpen = false;
			lastOpen = lastSwap = clock();
		}
		void Normalize() {
			for(short i = 0; i < ITEM_CNT; ++i)
				items[i].Normalize();
		}
		void Clear() {
			cur=0;
			precur = -1;
			for(short i = 0; i < ITEM_CNT; ++i)
				items[i].Clear();
			tmpItem.Clear();
			tmpItemFrom = -1;
			isOpen = false;
			lastOpen = lastSwap = clock();
		}
		BYTE GetCurMode(short index) const {
			if(index == cur)
				return CM_CURRENT;
			else if(index == precur)
				return CM_PRECURRENT;
			return CM_NORMAL;
		}
		void DrawHandItem(long x, long y) {
			if(HandItem().id != 0) {
				//绘制物品名称
				setcolor(itemdata.at(HandItem().id).color);
				pxprint(x, y - ITEM_FS*2,itemdata.at(HandItem().id).cn_name);
			}
		}
		void DrawHotbar(long x, long y) {
			//绘制关闭背包时的快捷栏物品
			DrawHandItem(x,y);
			setcolor(LIGHTGRAY);
			setlinestyle(PS_SOLID,0,2);
			line(rui_left, 20, rui_left, uiy-20);
			for(short i = 0; i < HOTBAR_ITEM_CNT; ++i)
				items[i].Draw(x,y+i*ITEM_FS,GetCurMode(i),false);
		}
		Item& HandItem(void) {
			//引用
			if(cur < 0 || cur >= ITEM_CNT) {
				ErrorLogTip(NULL,"物品下标访问越界:"+ToString(cur)+">="+ToString(ITEM_CNT),"Inventory::HandItem");
				return items[0];
			}
			if(tmpItem.id != 0)	return tmpItem;
			return items[cur];
		}
		const Item& HandItemCRef(void) 	const {
			//常引用版
			if(cur < 0 || cur >= ITEM_CNT) {
				ErrorLogTip(NULL,"物品下标访问越界:"+ToString(cur)+">="+ToString(ITEM_CNT),"Inventory::HandItemCRef");
				return items[0];
			}
			if(tmpItem.id != 0)	return tmpItem;
			return items[cur];
		}
		bool HaveItem(const Item& item)	const {
			if(item.id == 0 || item.cnt <= 0)	return true;
			if(item.cnt == 1) {
				for(short i = 0; i < ITEM_CNT; ++i) {
					if(items[i].id == item.id)
						return true;
				}
				return false;
			} else {
				short cnt=0;
				for(short i = 0; i < ITEM_CNT; ++i) {
					if(items[i].id == item.id)
						cnt += items[i].cnt;
					if(cnt >= item.cnt)
						return true;
				}
				return false;
			}
		}
		void DrawInventory(long _x, long _y) {
			//绘制打开的背包
			DrawHandItem(_x,_y);
			setcolor(GREEN);
			setlinestyle(PS_DASHDOT,0,2);
			line(_x + SLOT_W - 7, _y + 2, _x + SLOT_W - 7, _y + 2 + ITEM_FS*HOTBAR_ITEM_CNT);
			line(_x + SLOT_W*2 - 7, _y + 2, _x + SLOT_W*2 - 7, _y + 2 + ITEM_FS*HOTBAR_ITEM_CNT);
			line(_x + SLOT_W*3 - 7, _y + 2, _x + SLOT_W*3 - 7, _y + 2 + ITEM_FS*HOTBAR_ITEM_CNT);
			for(short x=0; x < ITEM_CNT / HOTBAR_ITEM_CNT; ++x)
				for(short y = 0; y < HOTBAR_ITEM_CNT && x*HOTBAR_ITEM_CNT+y < ITEM_CNT; ++y)
					items[x*HOTBAR_ITEM_CNT+y].Draw(_x+x*SLOT_W,_y+y*ITEM_FS,GetCurMode(x*HOTBAR_ITEM_CNT+y),true);
		}
		void RefreshPrecur(long left,long top, long mx, long my) {
			if(!isOpen) {
				if(mx >= left && mx <= uix && my >= top && my <= top+ITEM_FS*HOTBAR_ITEM_CNT) {
					precur = (my - top) / ITEM_FS;
				} else {
					precur = -1;
				}
			} else {
				if(mx >= left && mx <= left + ITEM_CNT / HOTBAR_ITEM_CNT * SLOT_W && my >= top && my <= top+ITEM_FS*HOTBAR_ITEM_CNT) {
					short x_precur=0,y_precur=0;
					x_precur = (mx - left) / SLOT_W;
					y_precur = (my - top) / ITEM_FS;
					precur = x_precur * HOTBAR_ITEM_CNT + y_precur;
				} else {
					precur = -1;
				}
			}
		}
		void RandomInventory() {
			for(short i = 0; i < ITEM_CNT; ++i)
				items[i].RandomItem();
		}
};

struct OPMode {
	bool spectator;	//观察者模式

	OPMode() {
		Init();
	}
	void Init() {
		spectator = false;
	}
} opmode;

class CraftPlace {
	public:

		char kind;		//种类
		short level;	//等级

		CraftPlace() {
			Init();
		}
		void Init() {
			kind='?';
			level=0;
		}
		inline string ToString(void)	const {
			return "\""+::ToString(kind)+'.'+::ToString(level)+"\"";
		}
		inline bool IsThisOK(const CraftPlace& cp) const {
			return (!isalpha(this->kind) || cp.kind == this->kind && cp.level >= this->level);
//			DebugLog("kind="+::ToString(cp.kind)+"this->kind="+::ToString(this->kind)+" lvl="+::ToString(cp.level)+" this->lvl="+::ToString(this->level)+" ret="+::ToString(ret));
		}
};
CraftPlace ParseCraftPlace(const string& src, bool updateTotalPlaceCount);

#define CSYS_DST_FS (ITEM_FS*1.3f)
#define CSYS_DST_CUR_FS (ITEM_FS*1.9f)
#define CSYS_SRC_FS (ITEM_FS*1.2f)
#define SLOT_BGCOLOR EGERGB(42,51,48)
#define CSYS_LEFTBAR_W (CSYS_DST_FS*3)
typedef ULONG RECIPE_ID;
class Recipe {
	public:
		RECIPE_ID id;
		vector<Item> src;
		Item dst;

		CraftPlace place;

		Recipe() {
			place.Init();
		}
		bool IsThisOK(JSON * const json)	const {
			//CraftPlaces checking
			XCPT_L
			if(place.kind < 'A' or place.kind > 'Z')
				return true;	//no need craftplace
			if(!json->HaveKey("CraftPlaces"))
				return false;
			vector<string> cplaces;
			bool suc = json->GetComplexValue("CraftPlaces",&cplaces);
			if(!suc)	return false;
			if(!place.IsThisOK(ParseCraftPlace(unquote(cplaces.at(place.kind-'A')),false)))
				return false;
			return true;
			XCPT_R
		}
#define ITEM_CNT_CRAFT_RATIO 0.95f
		void Draw(int x, int y, bool selected) {
			if(src.empty() || dst.id == 0)
				return;
//			dst.Draw(x,y,CM_NORMAL,true);
			if(selected) {
				setfillcolor(EGERGB(30,30,30));
				bar(x-3,y-2,x+CSYS_DST_CUR_FS*1.4,y+CSYS_DST_CUR_FS);
				setfont(CSYS_DST_CUR_FS,0,"Determination Mono");
			} else
				setfont(CSYS_DST_FS,0,"Determination Mono");
			setcolor(itemdata.at(dst.id).color);
			if(itemdata.at(dst.id).shape.size()>0 && itemdata.at(dst.id).shape.at(0) != 'U')
				pxprint(x,y,itemdata.at(dst.id).shape);
			else {
				pxprint(x, y, HexToDec(strxhead(itemdata.at(dst.id).shape)));
			}
			if(dst.cnt > 1) {
				if(selected) {
					setfont(CSYS_DST_CUR_FS*ITEM_CNT_CRAFT_RATIO,0,"Determination Mono");
					setwcolor(PINK);
					pxprint(x+CSYS_DST_CUR_FS+10,y+CSYS_DST_CUR_FS-CSYS_DST_CUR_FS*ITEM_CNT_CRAFT_RATIO+1,ToString(dst.cnt));
				} else {
					setfont(CSYS_DST_FS*ITEM_CNT_CRAFT_RATIO,0,"Determination Mono");
					setwcolor(WHITE);
					pxprint(x+CSYS_DST_FS+10,y+CSYS_DST_FS*0.5,ToString(dst.cnt));
				}
			}
			if(selected) {
				for(short i = 0; i < src.size(); ++i) {
					//Source materials drawing
					setfillcolor(SLOT_BGCOLOR);
					bar(CSYS_LEFTBAR_W+(CSYS_SRC_FS+5)*i+7,y,CSYS_LEFTBAR_W+(CSYS_SRC_FS+5)*(i+1)+3,y+CSYS_SRC_FS+5);
					//											src.at(i).Draw(CSYS_LEFTBAR_W+CSYS_SRC_FS*i+10,y+2,(selected?CM_CURRENT:CM_NORMAL),true);
					setfont(CSYS_SRC_FS,0,"Determination Mono");
					setcolor(itemdata.at(src.at(i).id).color);
					if (itemdata.at(src.at(i).id).shape.size() > 0 && itemdata.at(src.at(i).id).shape.at(0) != 'U')
						pxprint(CSYS_LEFTBAR_W+(CSYS_SRC_FS+5)*i+10,y+2,itemdata.at(src.at(i).id).shape);	
					else
						pxprint(CSYS_LEFTBAR_W + (CSYS_SRC_FS + 5)*i + 10, y + 2, HexToDec(strxhead(itemdata.at(src.at(i).id).shape)));
					if(src.at(i).cnt > 1) {
						setfont(CSYS_SRC_FS*ITEM_CNT_CRAFT_RATIO,0,"Determination Mono");
						setwcolor(LIGHTGRAY);
						pxprint(CSYS_LEFTBAR_W+(CSYS_SRC_FS+5)*i+10+CSYS_SRC_FS-textwidth(ToString(dst.cnt).c_str())*0.9f,y+CSYS_SRC_FS-CSYS_SRC_FS*0.4,ToString(src.at(i).cnt));
					}
				}
			}
		}
};
vector<Recipe> recipes;

void ReadRecipes() {
	if(!ExistFile(RECIPE_PATH)) {
		ErrorLogTip(NULL,"配方数据丢失","ReadRecipes");
		return;
	}
	DebugLog("【读取】读取配方数据："+RECIPE_PATH);
	vector<string> lines = ReadFileLines(RECIPE_PATH);
	long i=1;
	while(i < lines.size()) {
		string line = lines.at(i);
		if(line.empty()) {
			++i;
			continue;
		}

		stringstream ss;
		Recipe rcp;
		ss<<line;
		ss>>rcp.id;
		if(rcp.id == 0) {
			recipes.push_back(rcp);
			++i;
			continue;
		}
		string tmp;
		ss>>tmp;
		vector<string> cut = CutLine(tmp,'*');
		if(cut.size() == 1) {
			rcp.dst.id = CnNameToItem(cut[0]);
			rcp.dst.cnt = 1;
		} else {
			rcp.dst.id = CnNameToItem(cut[0]);
			rcp.dst.cnt = Convert<string,short>(cut[1]);
		}
		rcp.dst.AdaptDurability();

		ss>>tmp;	//'='

		while(!ss.eof() && tmp != "@") {
			//sources list
			ss>>tmp;
			if(tmp == "@")
				break;
			Item item;
			cut = CutLine(tmp,'*');
			if(cut.size() == 1) {
				item.id = CnNameToItem(cut[0]);
				item.cnt = 1;
			} else {
				item.id = CnNameToItem(cut[0]);
				item.cnt = Convert<string,short>(cut[1]);
			}
			rcp.src.push_back(item);
		}
		if(tmp == "@" && !ss.eof()) {
			ss>>tmp;
			rcp.place = ParseCraftPlace(tmp,true);
		}

		recipes.push_back(rcp);
		++i;
	}
}
short csys_item_cnt_up=4;
#define CSYS_ITEM_TOP (100 * (uiy / 1080.0))
#define CSYS_ITEM_CNT (csys_item_cnt_up*2+1)
#define CSYS_ITEM_H	(CSYS_DST_FS*1.4f)
#define CSYS_FIRST_CRAFT_CD 900
#define CSYS_FIRST_CRAFT_CD_MAX 2500
#define CSYS_CRAFT_CD 100
#define CSYS_CHOOSE_NEAR_RECIPE_CD 350

#define CSYS_TITLE_FS (55 * (uiy / 1080.0))
#define CSYS_CRAFT_FS (30 * (uiy / 1080.0))
#define CSYS_TIP_FS (30 * (uiy / 1080.0))
void AdaptCraftItemCount(void) {
	csys_item_cnt_up = (uiy*0.4/CSYS_ITEM_H);
	/*if(csys_item_cnt % 2 == 1)
		csys_item_cnt--;*/
}
short total_kind_cnt=0;
class CraftSystem {
	public:
		vector<RECIPE_ID> active_recipes;
		short top_cur;	//顶部下标

		short firstCrafted;	//a flag about crafting
		clock_t lastFirstCraft;
		clock_t lastCraft;
		clock_t lastChooseNearRecipe;
		RECIPE_ID lastRecipe;
		bool waitForRelease;	//长按阻断器

		CraftSystem() {
			lastRecipe = 0;
			waitForRelease = false;
			firstCrafted = 0;
			top_cur = 0;
			lastFirstCraft = lastCraft = lastChooseNearRecipe = clock();
		}
		inline short Cur(void) const {
			return top_cur + csys_item_cnt_up;
		}
		void RefreshPrecur(void);
		void Draw(void) {
			//CraftSystem::
			setfillcolor(BLACK);
			bar(0,0,CSYS_LEFTBAR_W,uiy);
			setcolor(YELLOW);
			setlinestyle(PS_SOLID,0,2);
			line(CSYS_LEFTBAR_W,0,CSYS_LEFTBAR_W,uiy);

			setfont(CSYS_TITLE_FS,0,"Determination Mono");
			setwcolor(YELLOW);
			pxprint(10,20,"合成");
			setfont(CSYS_CRAFT_FS,0,"Determination Mono");
			setwcolor(WHITE);
			if(!active_recipes.empty())
				pxprint(10,80,ToString(active_recipes.size())+"个可用配方");
			else
				pxprint(10,100,"没有可用配方!");
			setwcolor(DARKGRAY);
			setfont(CSYS_TIP_FS,0,"Determination Mono");
			pxprint(10,uiy- CSYS_TIP_FS *2,"< >:选择项目");
			pxprint(10,uiy- CSYS_TIP_FS,"Enter:合成");

//			for(short i = 0; i < CSYS_ITEM_CNT && top_cur+i < active_recipes.size(); ++i)
			for(short i = 0; i < CSYS_ITEM_CNT; ++i) {
				if(top_cur+i < 0)	continue;
				if(top_cur+i >= active_recipes.size())	break;
				recipes.at(active_recipes[top_cur+i]).Draw(20,CSYS_ITEM_TOP+CSYS_ITEM_H*i
				        + (i > CSYS_ITEM_CNT/2 ? CSYS_DST_CUR_FS-CSYS_DST_FS:0),
				        top_cur+i == Cur());
			}
		}
		inline void LimitTopCur(void) {
			ClampA<short>(top_cur,-csys_item_cnt_up, active_recipes.size()-csys_item_cnt_up-1);
		}
		void MoveTopCur(short change) {
			top_cur += change;
			LimitTopCur();
		}
		bool IsActiveRecipe(RECIPE_ID rid, Inventory* const bag, JSON* const json) const {
			if(!recipes.at(rid).IsThisOK(json)) {
				return false;
			}
			for(short j = 0; j < recipes.at(rid).src.size(); ++j) {
				if(!bag->HaveItem(recipes.at(rid).src.at(j))) {
					return false;
				}
			}
			return true;
		}
		bool CraftOnce(RECIPE_ID rid, Inventory* bag);
};

typedef USHORT ACHV_ID;
typedef USHORT ACOND_TYPE;
#define ACDT_NULL 0
#define ACDT_DIG 1	//摧毁方块 
#define ACDT_GET 2	//获得物品 
#define ACDT_KILL 3	//杀死实体
#define ACDT_USE 4	//使用物品 
class AchvCondition
{		//成就触发条件 
	public:
		ACOND_TYPE type;
		string data1;
		string data2;
		
	AchvCondition()
	{
		type = ACDT_NULL;
		data1="$";
		data2="$";
	}
	bool ThisOK(const Item& item) const
	{
		if(data1 == "id" && item.id == Convert<string,short>(data2))
		    return true;	//get_id_xxx
        if(data1 == "tag" && item.HaveTag(data2))
            return true;	//get_tag_xxx
        if(data1 == "type" && item.GetType() == ParseItemType(data2))
        	return true;	//get_type_xxxx
        if(data1 == "bclass" && item.GetType() == ITT_BLOCK 
		   && blockdata.at(ItemToBlock(item.id)).HaveClass(Convert<string,short>(data2)))
		   return true;	//get_bclass_xxx 仅限于方块物品 
        return false;
	}
	inline bool GetThisOK(const Item& item) const
	{
		if(type != ACDT_GET)	return false;
		return ThisOK(item);
	}
	inline bool UseThisOK(const Item& item) const
	{
		if(type != ACDT_USE)	return false;
		return ThisOK(item);
	}
	bool DigThisOK(BLOCK_ID bid) const
	{
		if(type != ACDT_DIG)	return false;
		if(data1 == "id" && bid == Convert<string,USHORT>(data2))
		    return true;	//dig_id_xx
        if(data1 == "bclass" && blockdata.at(bid).HaveClass(ParseBlockClass(data2)))
            return true;	//dig_bclass_xxx
        return false;
	}
	bool KillThisOK(const Entity& et) const
	{
		if(type != ACDT_KILL)	return false;
		if(data1 == "id" && et.id == Convert<string,short>(data2))
			return true;	//kill_id_xx
		if(data1 == "type" && et.GetType() == ParseEntityType(data2))
			return true;	//kill_type_xxxx
		return false;
	}
};
struct AchievementData
{	//成就数据 
	ACHV_ID id;
	string cn_name;
	string shape;
	color_t color;
	string definition;
	string description;
	
	AchvCondition cond;
	bool PartialCheckIsOK() const;
};
vector<AchievementData> achievement_data;
void CheckAchvGets();

AchvCondition ParseAchvCondition(const string& src)
{
	XCPT_L
	AchvCondition res;
	if(src.empty() || src=="$")	return res;
	vector<string> cut = CutLine(src,'_');
	if(cut.empty())	return res;
	//Approach Achievement Condition Type
	if(cut[0] == "get" && cut.size() > 1)
		res.type = ACDT_GET;
	else if(cut[0] == "dig" && cut.size() > 1)
		res.type = ACDT_DIG;
	else if(cut[0] == "kill" && cut.size() > 1)
		res.type = ACDT_KILL;
	else if(cut[0] == "use" && cut.size() > 1)
		res.type = ACDT_USE;
	else
	{
		ErrorLogTip(NULL,"无效的成就触发条件: "+src,"ParseAchvCondition");
		res.type = ACDT_NULL;	
	}
	//Approach other data
	res.data1 = cut[1];
	if(cut.size() > 2)
	{	//统一处理 
		res.data2 = cut[2];
	}
	return res;
	XCPT_R
}
void ReadAchievementData()
{
	if(!ExistFile(ACHVDATA_PATH)) {
		ErrorLogTip(NULL,"成就数据文件丢失！","ReadAchievementData");
		return;
	}
	DebugLog("【读取】读取成就数据："+ACHVDATA_PATH);
	vector<string> lines = ReadFileLines(ACHVDATA_PATH);
	long i=1;
	while(i < lines.size()) {
		string line = lines.at(i);
		if(line.empty()) {
			++i;
			continue;
		}
		line = CSVResplitLine(line);

		stringstream ss;
		AchievementData acdt;
		ss<<line;
		ss>>acdt.id;
		
		if(acdt.id == 0)
		{
			achievement_data.push_back(acdt);
			++i;
			continue;
		}
		ss>>acdt.cn_name;
		ss>>acdt.shape;
		
		string tmp;
		ss>>tmp;
		acdt.color = ParseColor(tmp);
		ss>>tmp;
		acdt.cond = ParseAchvCondition(tmp);
		
		ss>>acdt.definition;
		ss>>acdt.description;
		
		achievement_data.push_back(acdt);
		++i;
	}
}
vector<ACHV_ID> ParseAchievements(const string& src)
{
	XCPT_L
	vector<ACHV_ID> res;
	if("list" != ::GetValueType(src))
	   return res;
    vector<string> raw = ParseList(src,"ParseAchievements");
    if(raw.empty())	return res;
    for(short a=0;a<raw.size();++a)
    	res.push_back(Convert<string,USHORT>(raw.at(a)));
   	return res;
	XCPT_R
}

#define GAIN_SHOW_TIME 4000
#define GAIN_FADE_TIME 800
#define GAIN_ENTER_TIME 300
#define GAIN_QUEUE_MAX_SIZE 8
#define TIP_SHOW_TIME 6000
struct GainItem
{
	Item item;
	clock_t start;

	float GetShowRatio(void) const
	{
		if (clock() - start < GAIN_SHOW_TIME - GAIN_FADE_TIME)
			return 1.0f;
		if (clock() - start > GAIN_SHOW_TIME)
			return 0.0f;
		return 1.0f - (clock() - start - GAIN_SHOW_TIME + GAIN_FADE_TIME) / (float)GAIN_FADE_TIME;
	}
	float GetEnterRatio(void) const
	{
		if (clock() - start >= GAIN_ENTER_TIME)
			return 1.0f;
		return (clock() - start) / float(GAIN_ENTER_TIME);
	}
};
using TONE = USHORT;
class PlayerAboveTip
{
public:
	queue<GainItem> gains;
	string tip;
	color_t tip_color;
	clock_t tip_start;

	static int tip_fs;
	static int gain_fs;

	PlayerAboveTip()
	{
		tip = "";
		tip_start = clock();
		tip_color = YELLOW;
	}
#define TONE_NULL 0
#define TONE_HAGGLE 1
#define TONE_HAGGLE_1 2
#define TONE_HAGGLE_2 3
#define TONE_HAGGLE_3 4
#define TONE_IDLE 5
#define TONE_IDLE_1 6
#define TONE_IDLE_2 7
#define TONE_IDLE_3 8
#define TONE_NO 9
#define TONE_NO_1 10
#define TONE_NO_2 11
#define TONE_NO_3 12
#define TONE_YES 13
#define TONE_YES_1 14
#define TONE_YES_2 15
#define TONE_YES_3 16
#define TONE_HIT 17
#define TONE_HIT_1 18
#define TONE_HIT_2 19
#define TONE_HIT_3 20
#define TONE_DEATH 21
	void SetTip(const string& s, TONE tone, color_t tipColor);

	void AddGain(const Item& _item)
	{
		XCPT_L
		GainItem gi;
		gi.item = _item;
		auto q2{ gains };
		queue<GainItem> qneo;
		while (!q2.empty())
		{
			if (q2.front().item.id == _item.id /* tag暂不考虑 */)
			{
				gi.item.cnt += q2.front().item.cnt;
			}else
				qneo.push(q2.front());
			q2.pop();
		}
		gains = qneo;	//replace
		gi.start = clock();
		gains.push(gi);
		XCPT_R
	}
	void Update()
	{
		XCPT_L
		if (!tip.empty() && clock() - tip_start > TIP_SHOW_TIME)
		{	//clear the tip
			tip = "";
			tip_color = WHITE;
		}
		while (!gains.empty() && gains.size() > GAIN_QUEUE_MAX_SIZE)
			gains.pop();
		if (!gains.empty() && fequ(gains.front().GetShowRatio(), 0.0f))
		{
			gains.pop();
		}
		XCPT_R
	}
	void Draw()
	{
		XCPT_L
		if (!tip.empty())
		{
			setfont(tip_fs, 0, "Determination Mono");
			setwcolor(tip_color);
			midpxprint_shadow(tip, GetPlayerDrawTop() - tip_fs - 7*(uiy/1080.0), scene_left, scene_right);
		}
		auto q2{ gains };
		while (!q2.empty())
		{
			GainItem gi = q2.front();
			int _y=0;
			int _end = GetPlayerDrawTop() - (gain_fs + 3*(uiy/1080.0)) * q2.size();
			_y = Lerp<int>(GetPlayerDrawTop() - 3 * (uiy / 1080.0), _end, gi.GetEnterRatio());

			setfont(gain_fs, 0, "Determination Mono");
			pxsetalpha(0xFF * gi.GetShowRatio());
			//setwcolor(EGEACOLOR(0xFF * (1.0 - gi.GetShowRatio()), EGERGB(220, 220, 220)));
			setwcolor(EGERGB(220, 220, 220));
			string text = itemdata.at(gi.item.id).cn_name;
			if (gi.item.cnt > 1)
				text += "x" + ToString(gi.item.cnt);
			midpxprint_shadow(text, _y, scene_left, scene_right);
			pxsetalpha();
			q2.pop();
		}
		XCPT_R
	}
};
int PlayerAboveTip::tip_fs = 35;
int PlayerAboveTip::gain_fs = 33;
void OnBlockPXChanged(void)
{
	PlayerAboveTip::tip_fs = Clamp<int>(35 * (g_block_px / DEF_BLOCK_INIT_PX),20,50);
	PlayerAboveTip::gain_fs = Clamp<int>(33 * (g_block_px / DEF_BLOCK_INIT_PX),20,50);
}

#define PL_DEF_MAX_HP 50
#define PL_DEF_MAX_MP 10
#define PL_DEF_MAX_SP 50
#define PL_DEF_NAME "Player"
#define PL_DEF_SHAPE_ID 0x263B
#define PL_DEF_COLOR YELLOW
#define PL_DEF_MOVE_CD 100
#define PL_DEF_REACH_RANGE 3.6
#define PL_HURT_TIME 400
#define PL_LUCK_INIT_MIN 40
#define PL_LUCK_INIT_MAX 60
#define RESPAWN_CD 7000
#define DISCARD_CD 300
#define OPERATE_CD 300
#define ACHV_SHOWN_TIME 4000

class Player : public Creature{	//玩家类
	public:
		string name;	//玩家名

		long shapeID;
		color_t color;

		//血量
		short max_hp;
		//魔法值
		short mp;
		short max_mp;
		//饱腹值
		short sp;
		short max_sp;

		short luck;	//幸运度(0-100) 50为均值 每天都在40-60波动
		//背包
		Inventory bag;
		CraftSystem craft;
		ChatBox chatbox;
		bool chatOpen;	//是否打开聊天栏

		//指针
		Block* preblock;	//鼠标指向的方块 
		Block* inspblock;	//查看的方块 
		LCOORD preblock_coord;
		LCOORD inspblock_coord;
		LCOORD lastPutOrDigBlock_coord;
		LCOORD lastOperateBlock_coord;
		LCOORD lastChunkCoord;

		//时间戳
		clock_t lastDiscard;
		clock_t lastHurt;
		clock_t lastDeath;
		clock_t lastHPRegen;
		clock_t lastMPRegen;
		clock_t lastSPDecrease;
		clock_t lastSPHurt;
		clock_t lastAchieve;
		clock_t lastOperate;
		short sp_step_cnter;
		bool dead;

		//其他数据
		JSON json;
		vector<ACHV_ID> achievements;
		ACHV_ID prevAchv;
		PlayerAboveTip aboveTip;

		static const vector<string> hurtSayings;
		static const vector<string> deathSayings;

		Player() {
			Init();
		}
		~Player()
		{
			achievements.clear();
			preblock = inspblock = nullptr;
		}
		void Init() {
			name=PL_DEF_NAME;
			shapeID=PL_DEF_SHAPE_ID;
			color=PL_DEF_COLOR;
			x=y=0;
			uuid=0L;
			dir = RIGHT;
			hp=max_hp=PL_DEF_MAX_HP;
			mp=max_mp=PL_DEF_MAX_MP;
			sp=max_sp=PL_DEF_MAX_SP;
			preblock = inspblock = nullptr;
			prevAchv = 0;
			lastMove=lastAttack=lastHurt=lastSPHurt=lastDiscard=clock();
			lastHPRegen=lastOperate=lastMPRegen=lastSPDecrease=lastAchieve=clock();
			lastDeath=clock() - RESPAWN_CD;
			sp_step_cnter=0;
			dead=false;
			chatOpen = false;
			luck = 50;
			lastChunkCoord = { 0,0 };
		}
		inline bool IsMoving(void) const
		{
			return (clock() - lastMove <= GetMoveCD());
		}
		inline bool IsBeingHurt(void) const
		{
			return (clock() - lastHurt <= PL_HURT_TIME);
		}
		inline bool HaveAchieved(ACHV_ID aid) const
		{
			for(short a = 0; a < achievements.size(); ++a)
			    if(achievements.at(a) == aid)
			        return true;
            return false;
		}
		void HurtSound()
		{
			if (options.player_mutter)
			{
				aboveTip.SetTip(Choice(hurtSayings), TONE_HIT, YELLOW);
			}
			else
				SND("hurt");
		}
		void DeathSound()
		{
			if (options.player_mutter)
				aboveTip.SetTip(Choice(deathSayings), TONE_DEATH, PINK);
			else
				SND("death");
		}
		void Achieve(ACHV_ID aid)
		{
			SND("achieve");
			prevAchv = aid;
			lastAchieve = clock();
			DebugLog("玩家"+name+"获得成就:"+ToString(aid)+" "+achievement_data.at(aid).cn_name);
			achievements.push_back(aid);
		}
		void UpdateCraftPlaces() {
			XCPT_L
			vector<CraftPlace> places;
			places.resize(total_kind_cnt);
			for(char c = 'A'; c < 'A'+total_kind_cnt; ++c) {
				//Initialization
				places.at(c-'A').kind = c;
				places.at(c-'A').level = 0;
			}
			for(short _y = -1; _y <= 1; _y++)
				for(short _x = -1; _x <= 1; _x++) {
					long abs_x = this->x + _x;
					long abs_y = this->y + _y;
					if(OuttaWorld(abs_x,abs_y))
						continue;
//					if(world.GetBlock(abs_x,abs_y,"Player::UpdateCraftPlaces").HaveClass(BCLASS_CRAFT)) {
					if(blockdata.at(world.GetBlock(abs_x,abs_y,"Player::UpdateCraftPlaces").id).data.HaveKey("CraftPlace")) {
						//Craftable
						string s="";
						bool suc = blockdata.at(world.GetBlock(abs_x,abs_y,"Player::UpdateCraftPlaces").id).data.GetSimpleValue<string>("CraftPlace",&s);
						if(!suc) {
							//获取数据出现问题
							continue;
						}
						CraftPlace cp = ParseCraftPlace(s,false);
						if(CRAFTTT(cp.kind) == CT_BURNING && !world.GetBlock(abs_x,abs_y,"Player::UpdateCraftPlaces").HasFuel())
							continue;
						if(cp.level > places.at(cp.kind-'A').level) {
							//higher lvl
							places.at(cp.kind-'A').level = cp.level;	//update it
						}
					}
				}
			vector<string> splaces;
			for(short i = 0; i < places.size(); ++i) {
				//转成字符串列表
				splaces.push_back(places.at(i).ToString());
			}
			if(!json.HaveKey("CraftPlaces"))
				json.AddItem("CraftPlaces","[]");
			json.SetList("CraftPlaces", splaces);
			XCPT_R
		}
		void OnItemSwitch(int orig_index, int neo_index)
		{
			ITEM_ID orig_id = bag.items[orig_index].id;
			ITEM_ID neo_id = bag.items[neo_index].id;
			for (size_t i = 0; i < entities.size(); ++i)
			{
				if (entities.at(i).GetType() == ETT_BLADE
					&& entities.at(i).GetOwnerUUID() == this->uuid
					&& entities.at(i).tag.GetShort("FromItemID") == orig_id)
				{	//换手上物品时，原正在挥舞的实体将直接被删除
					auto iter = entities.begin();
					long j = 0;
					while (iter != entities.end() && j != i) {
						iter++;
						j++;
					}
					iter = entities.erase(iter);
					continue;
				}
			}
		}
		void OnChunkChanged(void)
		{
			world.manager.Update(x, y);
			lastChunkCoord = LCOORD{ ToChunkCoord(x),ToChunkCoord(y) };
		}
		void OnBladeHit(ITEM_ID iid_of_blade)
		{
			if (bag.HandItemCRef().id == iid_of_blade)
			{	//Only when hitting sth that blade should be damaged
				bag.HandItem().Damage(1);
			}
		}
		void PlayTone(TONE tone)
		{
			if (tone == 0)	return;
			string name = "death";
			if (tone == TONE_HAGGLE)
			{
				name = "haggle" + ToString(RandomRange(1, 3));
			}
			else if (tone == TONE_IDLE)
			{
				name = "idle" + ToString(RandomRange(1, 3));
			}
			else if (tone == TONE_NO)
			{
				name = "no" + ToString(RandomRange(1, 3));
			}
			else if (tone == TONE_YES)
			{
				name = "yes" + ToString(RandomRange(1, 3));
			}
			else if (tone == TONE_HIT)
			{
				name = "hit" + ToString(RandomRange(1, 3));
			}
			else if (tone == TONE_HAGGLE_1) {
				name = "haggle1";
			}
			else if (tone == TONE_HAGGLE_2) {
				name = "haggle2";
			}
			else if (tone == TONE_HAGGLE_3) {
				name = "haggle3";
			}
			else if (tone == TONE_IDLE_1) {
				name = "idle1";
			}
			else if (tone == TONE_IDLE_2) {
				name = "idle2";
			}
			else if (tone == TONE_IDLE_3) {
				name = "idle3";
			}
			else if (tone == TONE_NO_1) {
				name = "no1";
			}
			else if (tone == TONE_NO_2) {
				name = "no2";
			}
			else if (tone == TONE_NO_3) {
				name = "no3";
			}
			else if (tone == TONE_YES_1) {
				name = "yes1";
			}
			else if (tone == TONE_YES_2) {
				name = "yes2";
			}
			else if (tone == TONE_YES_3) {
				name = "yes3";
			}
			else if (tone == TONE_HIT_1) {
				name = "hit1";
			}
			else if (tone == TONE_HIT_2) {
				name = "hit2";
			}
			else if (tone == TONE_HIT_3) {
				name = "hit3";
			}
			else if (tone == TONE_DEATH)
			{
				name = "death";
			}
			SND("role\\" + name);
		}
		void Hurt(short damage, bool isFXHurt=false)
		{
			XCPT_L
			if(!TimeToHurt() || dead)
				return;		//处于无敌帧 或 已经死了
			HurtSound();
			hp -= damage;
			ClampA<short>(hp,0,max_hp);
			CreateDamageIndicator(x, y, damage, (isFXHurt?DIS_EFFECT:DIS_CRIT), hp <= 0);
			lastHurt = clock();
//			DebugLog("Hurt="+ToString(damage));
			
			XCPT_R
		}
#define OVER_SP_RATIO 1.5f
#define PL_HUNGRY_RATIO 0.3f
#define PL_VERY_HUNGRY_RATIO 0.1f
#define PL_FULL_RATIO 0.95f
#define PL_OVERFULL_RATIO (OVER_SP_RATIO*0.9f)
		//Stomach Status
#define HS_DEATH 0
#define HS_VERY_HUNGRY 1
#define HS_HUNGRY 2
#define HS_WELL 3
#define HS_FULL 4
#define HS_OVERFULL 5
		inline short GetHungryStatus(void) const
		{
			if (sp <= 0)
				return HS_DEATH;
			else if (sp <= max_sp * PL_VERY_HUNGRY_RATIO)
				return HS_VERY_HUNGRY;
			else if (sp <= max_sp * PL_HUNGRY_RATIO)
				return HS_HUNGRY;
			else if (sp <= max_sp * PL_FULL_RATIO)
				return HS_WELL;
			else if (sp <= max_sp * PL_OVERFULL_RATIO)
				return HS_FULL;
			return HS_OVERFULL;
		}
		void SaturTip()
		{
			if (sp >= max_sp * OVER_SP_RATIO)
			{
				aboveTip.SetTip("真的到极限了。", TONE_HIT, PURPLE);
			}
			else
			{
				auto hs = GetHungryStatus();
				if (hs == HS_OVERFULL)
				{
					aboveTip.SetTip(Choice({ "太撑了。", "我应该克制一下", "吃货也得注意极限！", "吃得太多不好吧。" }), TONE_NO, PINK);
				}
				else if (hs == HS_FULL)
				{
					aboveTip.SetTip(Choice({ "吃饱了", "饱了。", "不饿了。", "不错" }), TONE_YES, ORANGE);
				}
				else if (hs == HS_WELL)
				{
					aboveTip.SetTip(Choice({ "感觉还行。", "emm...", "好！" }), TONE_IDLE, GOLD);
				}
				else if (hs == HS_HUNGRY)
				{
					aboveTip.SetTip(Choice({ "饿！", "肚子饿了", "真饿啊。" , "饭点到了", "饿了"}), TONE_NO, YELLOW);
				}
				else if (hs == HS_VERY_HUNGRY)
				{
					aboveTip.SetTip(Choice({ "俺快要饿死了", "我不能死！", "别饿死...", "这样不行！", "肚子快要坍缩了" , "必须找东西下肚！"}), TONE_HIT, CORNSILK);
				}
				else {
					aboveTip.SetTip(Choice({ "没想到我的下场是饿死。", "阿门。", "菩萨保佑...", "阿弥陀佛！不能死", "终将死于饥馑。" }), TONE_HIT, WHITE);
				}
			}
		}
		bool Eat(short food)
		{
			if(sp >= max_sp*OVER_SP_RATIO)
			{
				return false;
			}
			SND("eat");
			auto hs_1 = GetHungryStatus();
			sp += food;
			if (sp > max_sp)
			{
				SND("burp");
			}
			ClampA<short>(sp, 0, max_sp*OVER_SP_RATIO);
			auto hs_2 = GetHungryStatus();
			if(hs_2 != hs_1)
				SaturTip();
			else {
				aboveTip.SetTip(Choice({ "嗯~", "有滋有味", "emm", "不错", "hh" }), TONE_YES, YELLOW);
			}
			return true;
		}
		void Birth(void)
		{
			PlayTone(TONE_HAGGLE);
			x=world.manager.map_w/2;	//AT THE
			y=world.manager.map_h/2;	//CENTRE
			dir = RIGHT;
			hp=max_hp=PL_DEF_MAX_HP;
			mp=max_mp=PL_DEF_MAX_MP;
			sp=max_sp=PL_DEF_MAX_SP;
			uuid = AttainNewUUID();
			prevAchv = 0;
			lastAchieve = clock() - ACHV_SHOWN_TIME;
			preblock = nullptr;
			lastMove=lastAttack=lastHPRegen=lastMPRegen=lastSPDecrease=clock();
			lastDeath=clock() - RESPAWN_CD;
			sp_step_cnter=0;
			dead=false;
			if (!effects.empty())	effects.clear();

			bag.items[0].Reset(46, 1);
			bag.items[1].Reset(47, 1);
			bag.items[2].Reset(48, 1);
			bag.items[0].tag.AddItem("VanishCurse", "1s");
			bag.items[1].tag.AddItem("VanishCurse", "1s");
			bag.items[2].tag.AddItem("VanishCurse", "1s");
			//			bag.RandomInventory();
		}
		void FirstInit(string& pname) {
			//地图必须已经生成好了
			name=pname;
			shapeID=PL_DEF_SHAPE_ID;
			color=PL_DEF_COLOR;
			luck = RandomRange(PL_LUCK_INIT_MIN, PL_LUCK_INIT_MAX);
			
			Birth();
		}
		#define PL_HP_REGEN_SLOW_CD 10000
		#define PL_HP_REGEN_STILL_CD 6000
		#define PL_HP_HUNGRY_HURT_CD 3000
		#define PL_HP_VERY_HUNGRY_HURT_CD 500
		#define PL_HP_OVERFULL_HURT_CD 2000
		void CheckHealthChange()
		{
			short stomach = GetHungryStatus();
			if(stomach == HS_DEATH)
			{
				hp = 0;
				lastDeath = clock();
				return;
			}else if(stomach == HS_VERY_HUNGRY)
			{
				if(clock() - lastSPHurt > PL_HP_VERY_HUNGRY_HURT_CD)
				{
					hp -= 1;
					ClampA<short>(hp,0,max_hp);
					lastSPHurt = clock();
					//lastHurt = clock();
				}
			}else if(stomach == HS_HUNGRY)
			{
				if(clock() - lastSPHurt > PL_HP_HUNGRY_HURT_CD)
				{
					hp -= 1;
					ClampA<short>(hp,0,max_hp);
					lastSPHurt = clock();
					//lastHurt = clock();
				}
			}else if(stomach == HS_OVERFULL)
			{
				if(clock() - lastSPHurt > PL_HP_OVERFULL_HURT_CD)
				{
					hp -= 1;
					ClampA<short>(hp,0,max_hp);
					lastSPHurt = clock();
					//lastHurt = clock();
				}
			}else{
				float k = 1.0f;
				if(stomach == HS_FULL)	//饱腹 
					k = 0.7f;
				else if(stomach == HS_OVERFULL)	//过饱
					k = 1.8f; 
				if(clock() - lastHPRegen > k*(IsMoving() ? PL_HP_REGEN_SLOW_CD : PL_HP_REGEN_STILL_CD))
				{
					hp += 1;
					ClampA<short>(hp,0,max_hp);
					lastHPRegen = clock();
				}
			}
			    
		}
		#define PL_MP_REGEN_SLOW_CD 2000
		#define PL_MP_REGEN_STILL_CD 1000
		void CheckMagicChange()
		{
			if(clock() - lastMPRegen > (IsMoving() ? PL_MP_REGEN_SLOW_CD : PL_MP_REGEN_STILL_CD))
			{
				mp += 1;
				ClampA<short>(mp,0,max_mp);
				lastMPRegen = clock();
			}
		}
		#define PL_SP_DECREASE_CD 90000
		#define PL_SP_STEP_CNTER_VALUE 8
		void CheckSaturChange()
		{
			if(clock() - lastSPDecrease > PL_SP_DECREASE_CD)
			{
				auto hs_1 = GetHungryStatus();
				sp -= 1;
				auto hs_2 = GetHungryStatus();
				if (hs_1 != hs_2 && hs_2 < HS_WELL)
					SaturTip();	//饿的提示
				ClampA<short>(sp,0,max_sp*OVER_SP_RATIO);
				lastSPDecrease = clock();
			}
			if(sp_step_cnter >= PL_SP_STEP_CNTER_VALUE)
			{	//walking will decrease sp
				lastSPDecrease -= PL_SP_DECREASE_CD * 0.05f;
				sp_step_cnter = 0;
			}
		}
		void CheckIndexesChanges(void)
		{	//各项指标的变动 
			if(!IsAlive())	 return;
			CheckHealthChange();
			CheckSaturChange();
			CheckMagicChange();
		}
		inline Block& CurBlock(void) {
			return world.GetBlock(x,y,"Player::CurBlock");
		}
		inline const Block& CurBlockCRef(void) const {
			return world.GetBlock(x,y,"Player::CurBlockCRef");
		}
		float GetReachRange(void)	const {
			float rg = PL_DEF_REACH_RANGE;
			return rg;
		}
		inline bool CanReach(float dist)	const {
			return (dist <= GetReachRange());
		}
		#define PL_SP_HUNGRY_MOVECD_RATIO 1.08f
		#define PL_SP_VERY_HUNGRY_MOVECD_RATIO 1.45f
		#define PL_SP_VERY_OVERFULL_MOVECD_RATIO 1.36f
		clock_t GetMoveCD(void)	const {
			clock_t cd = PL_DEF_MOVE_CD;
			if(opmode.spectator)
				return 5;
			if(blockdata.at(CurBlockCRef().id).data.HaveKey("Viscidity")) {
				//具有粘性
				float vis=1.0f;
				bool suc = blockdata.at(CurBlockCRef().id).data.GetSimpleValue<float>("Viscidity",&vis);
				if(!suc) {
					ErrorLogTip(NULL,"无法获取液体粘度\nX="+ToString(x)+" Y="+ToString(y)+" id="+ToString(CurBlockCRef().id),"Player::GetMoveCD");
					return cd * vis;
				}
				cd *= vis;
			}
			short stomach = GetHungryStatus();
			if(stomach == HS_HUNGRY)
				cd *= PL_SP_HUNGRY_MOVECD_RATIO;
			else if(stomach == HS_VERY_HUNGRY)
				cd *= PL_SP_VERY_HUNGRY_MOVECD_RATIO;
			else if(stomach == HS_OVERFULL)
				cd *= PL_SP_VERY_OVERFULL_MOVECD_RATIO;
			cd *= FXMoveCDCoefficient();
			return cd;
		}
		clock_t GetUseCD(void) const {
			//if(opmode.spectator)
			//	return 0;
			return itemdata.at(bag.HandItemCRef().id).GetUseCD();
		}
		inline bool TimeToMove() const {
			return (clock() - lastMove >= GetMoveCD());
		}
		inline bool TimeToUse() const {
			return (clock() - lastAttack >= GetUseCD());
		}
		inline bool TimeToHurt() const {
			return (clock() - lastHurt >= PL_HURT_TIME);
		}
		void DropLoot()
		{	//全部掉落 每个人的痛
			SND("collapse");
			vector<LCOORD> dropCoords{ LCOORD{x,y} };
			for(short _y = -1; _y <= 1; ++_y)
				for (short _x = -1; _x <= 1; ++_x)
				{
					if (OuttaWorld(x + _x, y + _y)
						|| _x == 0  && _y == 0
						|| world.GetBlock(x + _x, y + _y, "Player::DropLoot").IsBarrier())
						continue;
					dropCoords.emplace_back(x + _x, y + _y);
				}
			for (size_t i = 0; i < ITEM_CNT; ++i)
			{
				if (bag.items[i].id == 0)	continue;
				const LCOORD& _coord = ChoiceRef(dropCoords);
				auto[ _x, _y ] = _coord;
				if(!bag.items[i].tag.HaveKey("VanishCurse") || bag.items[i].tag.GetShort("VanishCurse")==0)	//没有消失诅咒
					SpawnItem(_x, _y, bag.items[i].id, bag.items[i].cnt,
						bag.items[i].tag.ToString().c_str());
				bag.items[i].Lose( bag.items[i].cnt );
			}
		}
		void OnPlayerDeath()
		{
			DeathSound();
			if (options.death_drop)
				DropLoot();
			if(CurBlockCRef().id == 0)
			{	//如果是空的格子才放墓碑 
				world.SetBlock(x,y,RandomGravestone());
			}
			luck = RandomRange(35, 65);
			Save();
		}
		bool IsAlive(void)
		{
			if(!dead && hp <= 0)
			{
				OnPlayerDeath(); 
				lastDeath = clock();
				return !(dead = true);
			}
			return (hp > 0);
		}
		inline void LimitCoord(void) {
			ClampA(x,0L,world.manager.map_w-1);
			ClampA(y,0L,world.manager.map_h-1);
		}
		void TryToMove(DIR dir) {
			if(!TimeToMove())
				return;
			sp_step_cnter++;
			DirOffsetPos(x,y,dir,"Player::TryToMove");
			LimitCoord();
			this->dir = dir;
			lastMove = clock();
		}
		short GainItem(ITEM_ID id, short cnt, const char* tag=nullptr) {
			//Player::
			//返回剩余个数 成功则返回0
			SND("obtain");
			short cnt0 = cnt;
			for (short i = 0; i < ITEM_CNT; ++i) {
				if (bag.items[i].id == id
					//					&& (tag==nullptr || (!bag.items[i].tag.Empty() && bag.items[i].tag.ToString() == string(tag)))
					&& bag.items[i].cnt < bag.items[i].GetMaxHeap()) {
					short original_cnt = bag.items[i].cnt;
					short of_cnt = 0;
					bool of = bag.items[i].Gain(cnt, &of_cnt);
					if (of) {
						cnt -= bag.items[i].GetMaxHeap() - original_cnt;
					}
					if (cnt <= 0) {
						cnt = 0;
						break;
					}
				}
			}
			for(short i = 0; i < ITEM_CNT; ++i) {
				if(bag.items[i].id == 0) {
					if (i == bag.cur)
						OnItemSwitch(bag.cur, i);
					//空位
					bag.items[i].id = id;
					bag.items[i].cnt = cnt;
					if(tag)
						bag.items[i].tag = ParseJSON(tag,"Player::GainItem");
					else
						bag.items[i].AdaptDurability();

					if(bag.items[i].cnt > bag.items[i].GetMaxHeap()) {
						//溢出
						cnt = (bag.items[i].cnt - bag.items[i].GetMaxHeap());
						bag.items[i].cnt = bag.items[i].GetMaxHeap();
					} else {
						cnt = 0;
						break;
					}
				}
			}
			if (cnt != cnt0)
			{
				Item gain;
				gain.id = id;
				gain.cnt = cnt0 - cnt;
				gain.tag = ParseJSON(tag);
				aboveTip.AddGain(gain);
			}
			
			CheckAchvGets();
			return cnt;
		}
		void LoseMagic(short cost)
		{
			mp -= cost;
			ClampA<short>(mp, 0, max_mp);
		}
		short LoseItem(ITEM_ID id, short cnt)
		{
			//返回剩余个数 成功则返回0
			for(short i = 0; i < ITEM_CNT; ++i) {
				if(bag.items[i].id == id) {
					if(bag.items[i].cnt >= cnt)
					{
						bag.items[i].Lose(cnt);
						cnt = 0;
					}else{
						cnt -= bag.items[i].cnt;
						bag.items[i].Clear();
					}
					if(cnt <= 0) {
						cnt = 0;
						break;
					}
				} 
			}
//			DebugLog("cnt="+ToString(cnt));
			return cnt;
		}
		bool HaveItem(const Item& item)	const {
			return bag.HaveItem(item);
		}
		ITEM_ID GetProjectile(ITEM_TYPE lautype, bool* has)
		{
			ITEM_TYPE ptype = GetLauncherProjectileType(lautype);
			for(short i = 0; i < ITEM_CNT; ++i)
				if(bag.items[i].GetType() == ptype)
				{
					*has = true;
					return bag.items[i].id;
				}
			*has = false;
			return 0;
		}
		void UpdateCraftRecipes(void) {
			XCPT_L
			craft.active_recipes.clear();
			for(long i = 1; i < recipes.size(); ++i) {
				bool flag = craft.IsActiveRecipe(i, &bag, &json);
				if(flag) {
					craft.active_recipes.push_back(i);
				}
			}
			XCPT_R
		}
		void RefreshLuck(void)
		{
			luck = RandomRange(PL_LUCK_INIT_MIN, PL_LUCK_INIT_MAX);
		}
};
vector<Player> players;

Player* FindPlayer(long uuid)
{
	for (size_t j = 0; j < players.size(); ++j)
		if (players.at(j).uuid == uuid)
			return &players.at(j);
	return nullptr;
}
template <typename T>
void Effect::Update(T* cptr)
{
	if (id == 3)
	{	//中毒
		if (clock() - last >= EFF_INTOX_HURT_CD)
		{
			cptr->Hurt(1, true);
			cptr->HurtSound();
			last = clock();
		}
	}
}
pair<clock_t,short> CalcKnockback(KBLEVEL kblvl, long x, long y, DIR knockDir)
{
	clock_t delay = kblvl * 100;
	short kbDist = 0;
	while (delay >= 1000)
	{
		DirOffsetPos(x, y, knockDir, "CalcKnockback");
		if (world.GetBlock(x, y, "CalcKnockback").IsBarrier())
			break;
		++kbDist;
		delay -= 1000;
	}
	return make_pair(delay, kbDist);
}
const vector<string> Player::hurtSayings 
   = { "不！", "不要过来啊！", "疼！", "危", "痛" };
const vector<string> Player::deathSayings
= { "我会复仇的！", "可恶！", "让你得逞了！", "我不会屈服的！", "来世找你算账。" };
//<!>
#define p players.at(0)

bool Item::Damage(short decr/* = 1*/)
{
	if (Unbreakable())
		return false;	//无法破坏的标签
	//Item::耐久度减少
	if (HaveDurabilityItemType(GetType())) {
		short odur = GetDur();
		odur -= decr;
		if (odur <= 0) {	//爆掉了
			SND("break");
			p.luck += Choice({ -9,-5, 5, 9 });
			Clear();	//报废
		}
		else {			//耐久减少
			tag.SetShort("Durability", odur);
		}
		return true;
	}
	else {
		return false;
	}
}
void Entity::DropLoot()
{
	XCPT_L
		if (!options.creature_drop
			|| !entitydata.at(id).data.HaveKey("DropLoot")
			|| status == EST_DEAD)	return;
	vector<string> loots;
	bool suc = entitydata.at(id).data.GetComplexValue("DropLoot", &loots);
	if (!suc)
	{
		ErrorLogTip(NULL, "无法获取实体掉落物列表 id=" + ToString(id), "Entity::DropLoot");
		return;
	}
	if (loots.empty())	return;
	for (short j = 0; j < loots.size(); ++j)
	{
		string item = UncoverValue(loots.at(j), "string");
		if (item.empty())	continue;
		vector<string> cut = CutLine(item, '*');
		ITEM_ID iid = Convert<string, USHORT>(UncoverValue(cut[0], "short"));
		short cnt = 1;
		if (cut.size() > 1 and cut[1] != "")
		{
			//					cnt = Convert<string,short>(UncoverValue(cut[1],"short"));
			cnt = RangeChoice(cut[1]);
		}
		float chance = 1.0f;
		if (cut.size() > 2 and cut[2] != "")
		{
			chance = Convert<string, float>(UncoverValue(cut[2], "float"));
		}
		if(!fequ(chance, 1.0f))
			chance *= Clamp(p.luck / 50.0, 0.75, 1.25);
		if (Percent(chance * 100))
		{
			SpawnItem(x, y, iid, cnt, GetItemInitialTag(iid).c_str());
		}
	}
	XCPT_R
}
void PlayerAboveTip::SetTip(const string& s, TONE tone, color_t tipColor)
{
	XCPT_L
	tip = s;
	tip_color = tipColor;
	p.PlayTone(tone);
	tip_start = clock();
	XCPT_R
}
#define SUR_SND_W_HALF (PL_VIEW_W_HALF*1.5)
#define SUR_SND_H_HALF (PL_VIEW_H_HALF*1.5)
void World::UpdateSurroundingSounds(void)
{
	if (entities.empty() || !options.sound_switch)	return;
	for (auto& et : entities)
	{
		if (et.x >= GetPlayerX() - SUR_SND_W_HALF
			&& et.x <= GetPlayerX() + SUR_SND_W_HALF
			&& et.y >= GetPlayerY() - SUR_SND_H_HALF
			&& et.y <= GetPlayerY() + SUR_SND_H_HALF)
		{	//In Range
			if (et.GetType() == ETT_ANIMAL || et.GetType() == ETT_MONSTER)
			{
				if (!et.IsAlive() && !et.TimeToBeCleared()
					&& !et.tag.HaveKey("DeathShouted"))
				{	//Death
					et.tag.AddItem("DeathShouted", "1s");
					et.DeathSound();
				}
				else if (et.IsBeingHurt() && clock() - et.lastHurt <= 15)
				{	//Hurt
					et.HurtSound();
					et.lastSay = clock() + ET_SAY_CD;
				}
				else if (clock() - et.lastMove <= 20)
				{	//Step
					et.StepSound();
				}
				else if (clock() - et.lastSay >= ET_SAY_CD
					&& Percent(3))
				{	//Idle Say
					et.SaySound();
					et.lastSay = clock();
				}
			}
		}
	}
}
void World::UpdateTime(void)
{
	static const vector<string> dusk_sayings
	{ "天要黑了，我需要非常小心！" ,
	  "时间不早了！",
	  "太阳要落山了！",
	  "可怕的黑夜快降临了！",
	  "这么快白天就过去了？",
	  "天要黑了！",
      "太阳要落山了，我需要非常小心！",
	};
	static const vector<string> dawn_sayings
	{ "天快亮了呢。",
	  "终于要白天了。",
	  "夜晚结束了",
	  "太阳升起了。",
	  "又是新的一天。",
	};
	if (clock() - lastMinTick > 1000)
	{
		time++;
		lastMinTick = clock();
	}
	if (time >= 24 * 60)
		time = 0;	//昼夜循环 
	if (time >= Sunrise && time < Sunset && !sunrisen)
	{
		MUS("music\\dawn");	//as a tip
		p.aboveTip.SetTip(Choice(dawn_sayings), TONE_YES, YELLOW);
		sunrisen = true;
		p.RefreshLuck();
		Save();
	}
	else if (time >= Sunset && sunrisen)
	{
		MUS("music\\dusk");	//for players
		p.aboveTip.SetTip(Choice(dusk_sayings), TONE_NO, LIGHTCYAN);
		sunrisen = false;
		p.RefreshLuck();
	}
}
bool Block::Break(long x, long y)
{
	XCPT_L
	bool isContainer = HaveClass(BCLASS_CONTAINER);
	BLOCK_ID bid=id;
	if(p.inspblock == this)
	{	//否则会出事 
		CloseContainer();
	}
	JSON otag = this->tag;
	DropFruit(x,y);
	if (blockdata.at(bid).data.HaveKey("BrokenSound"))
		SND("block\\" + blockdata.at(bid).data.GetString("BrokenSound"));
	else if (blockdata.at(bid).data.HaveKey("BrokenSoundFamily"))
		PlayFamilySound(ParseSoundFamily(blockdata.at(bid).data.GetString("BrokenSoundFamily")), SNDFW_BROKEN);
	else
		PlayFamilySound(blockdata.at(bid).snd_family, SNDFW_BROKEN);
Clear();
	CheckAchvDigs(bid);
	vector<string> sdropItems;
	vector<string> sdropCounts;
	vector<string> sdropChances;
	vector<string> sdropTags;
	bool hasCnt = true, hasChance = true;
	if(blockdata.at(bid).data.HaveKey("DropItem")) {
		bool suc = blockdata.at(bid).data.GetComplexValue("DropItem",&sdropItems);
		if(!suc) {
			ErrorLogTip(NULL,"无法获取方块掉落物数据\nid="+ToString(bid),"Block::Damage");
			return false;
		}
		if(sdropItems.empty())	//不掉落
			return true;
		if(blockdata.at(bid).data.HaveKey("DropCount")) {
			//没有的话默认全为1
			suc = blockdata.at(bid).data.GetComplexValue("DropCount",&sdropCounts);
			if(!suc) {
				ErrorLogTip(NULL,"无法获取方块掉落物数量数据\nid="+ToString(bid),"Block::Damage");
				return false;
			} 
		} else 
			hasCnt = false;
		
//					DebugLog("iid="+ToString(iid)+" cnt="+ToString(cnt));
	} else {
		sdropItems.push_back(ToString(BlockToItem(bid))+"s");
		sdropTags.push_back(otag.ToString());	//注意：是物品方块标签 
        hasCnt = false;
	}
	if(isContainer && !IsKeeperContainer(bid))
	{	//非保持型容器被摧毁后会掉落所有内含物 
		if(otag.HaveKey("Content") && otag.GetValueType("Content") == "list")
		{
			vector<string> content;
			bool suc = otag.GetComplexValue("Content",&content);
			if(suc && !content.empty())
				DropContentItems(x, y, content);
		}
		sdropTags.clear();	//<!> directly clear it
	}
	
	if(blockdata.at(bid).data.HaveKey("DropChance"))
	{
		bool suc = blockdata.at(bid).data.GetComplexValue("DropChance",&sdropChances);
		if(!suc) {
			ErrorLogTip(NULL,"无法获取方块掉落物概率\nid="+ToString(bid),"Block::Damage");
			return false;
		}
	}else
		hasChance = false;
	//drop entities
	for(short i = 0; i < sdropItems.size(); ++i)
	{
		ITEM_ID iid = Convert<string,ITEM_ID>(UncoverValue(sdropItems.at(i),"short"));
		short cnt=1;
		if(hasCnt)
			cnt = RangeChoice(UncoverValue(sdropCounts.at(i),"string"));
		float chance=1.0f;
		if(hasChance)
			chance = Convert<string,float>(UncoverValue(sdropChances.at(i),"float"));
		if (!fequ(chance, 1.0f))
			chance *= Clamp(p.luck / 50.0, 0.75, 1.25);	//幸运值开始干涉
		if(iid == 0 || !Percent(chance*100))
			continue;
		SpawnItem(x, y, iid, cnt, (i < sdropTags.size() ? BlockTagForItemTag(sdropTags.at(i)).c_str() : nullptr));
//					entities.at(entities.size()-1).RandomBlockPos();
	}
	if(otag.HaveKey("EDrop"))
	{	//个性化标签 
		DebugLog(otag.ToString());
		vector<string> edrops;
		bool suc = otag.GetComplexValue("EDrop",&edrops);
		if(!suc || edrops.empty())
			return true;
		//DebugLog(ToString(edrops));
		for(short j = 0; j < edrops.size(); ++j)
		{
			vector<string> cut = CutLine(edrops.at(j),'*');
			if(cut.empty())	continue;
			ITEM_ID iid = Convert<string,ITEM_ID>(UncoverValue(cut[0],"short"));
			if(iid == 0)	continue;
			short cnt=1;
			string cntrg="";	//区间 
			if(cut.size() > 1)
			{
				cntrg = UncoverValue(cut[1],"string");
				cnt = RangeChoice(cntrg);
			}
			float chance=1.0f;	//概率 
			if(cut.size() > 2)
				chance = Convert<string,float>(UncoverValue(cut[2],"float"));
			if (!fequ(chance, 1.0f))
				chance *= Clamp(p.luck / 50.0, 0.75, 1.25);	//幸运值开始干涉
//			DebugLog("id="+ToString(iid)+" cnt="+ToString(cnt)+" chance="+ToString(chance));
			if(Percent(chance*100))
			{
				string etag,itag;
				etag = "{Item:"+ToString(iid)+"s,Count:"+ToString(cnt)+"s";
				if((itag = GetItemInitialTag(iid)) != "{}")
				{
					etag += ",tag:";
					etag += itag;
				}
				etag += "}";
				SpawnItem(x, y, etag.c_str());
//							entities.at(entities.size()-1).RandomBlockPos();
			}
		}
	}
	XCPT_R
}
void World::ApproachPlayerLightSource(void)
{	//手上的光源 !
	if(p.bag.HandItem().IsLightSource())
	{
		LIGHTNESS ln = p.bag.HandItem().GetOwnLightness();
		PointLight(ln,GetPlayerX(),GetPlayerY());
	}
}
void World::ApproachEntityLightSource(void)
{	//实体光源 !
	for(short j = 0; j < entities.size(); ++j)
	{
		if(!InRange( entities.at(j).x, entities.at(j).y,SURROUNDING_RANGE))
		    continue;
		if(entities.at(j).IsLightSource())
		{
			LIGHTNESS ln = entities.at(j).GetOwnLightness();
			PointLight(ln, entities.at(j).x, entities.at(j).y);
		}
	}
}
#define ENV_CHG_W_HALF (CHUNK_PX*2)
#define ENV_CHG_H_HALF (CHUNK_PX*2)
void World::UpdateEnvironmentChanges(void)
{
	if (wpid == WPID_RAINDROP)
	{	//雨天积水
		if (RandomRange(0, 1000, true, false) < 400)
		{
			repeat(10)
			{
				//DebugLog("1");
				long e = 0;
				long x, y;
				do {
					x = RandomRange(0, ENV_CHG_W_HALF*2)- ENV_CHG_W_HALF + p.x;
					y = RandomRange(0, ENV_CHG_H_HALF*2)- ENV_CHG_H_HALF + p.y;
					++e;
					if (e > 1000)	//防止卡死
						return;
				} while (OuttaWorld(x, y) || GetBlock(x, y, "World::UpdateEnvironmentChanges").IsBarrier()
					|| GetBlock(x, y, "World::UpdateEnvironmentChanges").id == EnNameToBlock("water"));
				BLOCK_ID id = GetBlock(x, y, "World::UpdateEnvironmentChanges").id;
				//DebugLog("2 id=" + ToString(int(id)));
				if (id == EnNameToBlock("wet_ground"))
				{
					SetBlock(x, y, EnNameToBlock("puddle"), biome_void, "World::UpdateEnvironmentChanges");
				}
				else if (id == EnNameToBlock("puddle"))
				{
					SetBlock(x, y, EnNameToBlock("water_pit"), biome_void, "World::UpdateEnvironmentChanges");
				}
				else if (id == EnNameToBlock("water_pit"))
				{
					SetBlock(x, y, EnNameToBlock("water"), biome_void, "World::UpdateEnvironmentChanges");
				}
				else {
					SetBlock(x, y, EnNameToBlock("wet_ground"), biome_void, "World::UpdateEnvironmentChanges");
				}
				//DebugLog("3 id=" + ToString(int(GetBlock(x, y, "World::UpdateEnvironmentChanges").id)));
			}
		}
	}
	else if (wpid == WPID_SNOWFLAKE)
	{	//雪天积雪
		if (!OuttaWorld(p.x,p.y) && RandomRange(0, 1000, true, false) < world.GetBlock(p.x,p.y,"World::UpdateEnvironmentChanges").biome == biome_iceland ? 100 : 500)
		{
			repeat(10)
			{
				long e = 0;
				long x, y;
				do {
					x = RandomRange(0, ENV_CHG_W_HALF*2) - ENV_CHG_W_HALF + p.x;
					y = RandomRange(0, ENV_CHG_H_HALF*2) - ENV_CHG_H_HALF + p.y;
					++e;
					if (e > 1000)	//防止卡死
						return;
				} while (OuttaWorld(x, y)
					|| GetBlock(x, y, "World::UpdateEnvironmentChanges").IsBarrier()
					       && blockdata.at(GetBlock(x, y, "World::UpdateEnvironmentChanges").id)
					                   .en_name.find("snow") == string::npos
					|| GetBlock(x, y, "World::UpdateEnvironmentChanges").HaveClass(BCLASS_LIQUID)
					|| GetBlock(x, y, "World::UpdateEnvironmentChanges").id == EnNameToBlock("snow_block"));
				BLOCK_ID id = GetBlock(x, y, "World::UpdateEnvironmentChanges").id;
				if (id == EnNameToBlock("snow_ground"))
				{
					SetBlock(x, y, EnNameToBlock("snow_layer"), biome_void, "World::UpdateEnvironmentChanges");
				}
				else if (id == EnNameToBlock("snow_layer"))
				{
					SetBlock(x, y, EnNameToBlock("thick_snow"), biome_void, "World::UpdateEnvironmentChanges");
				}
				else if (id == EnNameToBlock("thick_snow"))
				{
					SetBlock(x, y, EnNameToBlock("snow_block"), biome_void, "World::UpdateEnvironmentChanges");
				}
				else {
					SetBlock(x, y, EnNameToBlock("snow_ground"), biome_void, "World::UpdateEnvironmentChanges");
				}
			}
		}
	}
	else if (wpid == WPID_NONE)
	{
		if (RandomRange(0, 1000, true, false) < 800)
		{
			repeat(2)	//蒸发速度很慢的
			{
				long e = 0;
				long x, y;
				do {
					x = RandomRange(0, ENV_CHG_W_HALF*2) - ENV_CHG_W_HALF + p.x;
					y = RandomRange(0, ENV_CHG_H_HALF*2) - ENV_CHG_H_HALF + p.y;
					++e;
					if (e > 1000)	//防止卡死
						return;
				} while (OuttaWorld(x, y) || !GetBlock(x, y, "World::UpdateEnvironmentChanges").HaveClass(BCLASS_LIQUID)
					|| GetBlock(x, y, "World::UpdateEnvironmentChanges").id == EnNameToBlock("lava"));
				BLOCK_ID id = GetBlock(x, y, "World::UpdateEnvironmentChanges").id;
				if (id == EnNameToBlock("wet_ground"))
				{
					SetBlock(x, y, EnNameToBlock("air"), biome_void, "World::UpdateEnvironmentChanges");
				}
				else if (id == EnNameToBlock("puddle"))
				{
					SetBlock(x, y, EnNameToBlock("wet_ground"), biome_void, "World::UpdateEnvironmentChanges");
				}
				else if (id == EnNameToBlock("water_pit"))
				{
					SetBlock(x, y, EnNameToBlock("puddle"), biome_void, "World::UpdateEnvironmentChanges");
				}
				else if(time%400==0){	//完整水缩小概率极小
					SetBlock(x, y, EnNameToBlock("water_pit"), biome_void, "World::UpdateEnvironmentChanges");
				}
			}
		}
		if (!OuttaWorld(p.x,p.y) && RandomRange(0, 1000, true, false) < (world.GetBlock(p.x, p.y, "World::UpdateEnvironmentChanges").biome == biome_iceland ? 200 : 900))
		{
			repeat(15)	//融化
			{
				long e = 0;
				long x, y;
				do {
					x = RandomRange(0, ENV_CHG_W_HALF*2) - ENV_CHG_W_HALF + p.x;
					y = RandomRange(0, ENV_CHG_H_HALF*2) - ENV_CHG_H_HALF + p.y;
					++e;
					if (e > 1000)	//防止卡死
						return;
				} while (OuttaWorld(x, y) || !GetBlock(x, y, "World::UpdateEnvironmentChanges").HaveClass(BCLASS_LIQUID)
					|| GetBlock(x, y, "World::UpdateEnvironmentChanges").id == EnNameToBlock("lava"));
				BLOCK_ID id = GetBlock(x, y, "World::UpdateEnvironmentChanges").id;
				if (id == EnNameToBlock("snow_ground"))
				{
					SetBlock(x, y, EnNameToBlock("wet_ground"), biome_void, "World::UpdateEnvironmentChanges");
				}
				else if (id == EnNameToBlock("snow_layer"))
				{
					SetBlock(x, y, EnNameToBlock("puddle"), biome_void, "World::UpdateEnvironmentChanges");
				}
				else if (id == EnNameToBlock("thick_snow"))
				{
					SetBlock(x, y, EnNameToBlock("water_pit"), biome_void, "World::UpdateEnvironmentChanges");
				}
				else if (id == EnNameToBlock("snow_block")
					&& world.GetBlock(p.x, p.y, "World::UpdateEnvironmentChanges").biome != biome_iceland
					&& time % 400 == 0) {	//完整雪融化概率极小 且雪地不融化
					SetBlock(x, y, EnNameToBlock("thick_snow"), biome_void, "World::UpdateEnvironmentChanges");
				}
			}
		}
	}
	//以下是方块自然变化
#define BLOCK_CHANGE_H_HALF (PL_VIEW_H_HALF*2.0)
#define BLOCK_CHANGE_W_HALF (PL_VIEW_W_HALF*2.0)
#define BLOCK_CHANGE_DEF_PROB 2
	for (long rel_y = -BLOCK_CHANGE_H_HALF; rel_y <= BLOCK_CHANGE_H_HALF; ++rel_y)
		for (long rel_x = -BLOCK_CHANGE_W_HALF; rel_x <= BLOCK_CHANGE_W_HALF; ++rel_x) {
			long abs_x = GetPlayerX() + rel_x;
			long abs_y = GetPlayerY() + rel_y;
			if (OuttaWorld(abs_x, abs_y)
				|| world.GetBlock(abs_x, abs_y, "DrawSceneBlocks").HaveClass(BCLASS_AIR)
				)
				continue;	//跳过
			Block& bref = world.GetBlock(abs_x, abs_y, "DrawSceneBlocks");
			if (blockdata.at(bref.id).data.HaveKey("BlockChangeTo") && Percent(IsDay()?3:1))
			{
				if (!blockdata.at(bref.id).data.HaveKey("BlockChangeProb") && Percent(BLOCK_CHANGE_DEF_PROB)
					|| Permille(1000*blockdata.at(bref.id).data.GetFloat("BlockChangeProb")))
				{
					BLOCK_ID bid = blockdata.at(bref.id).data.GetShort("BlockChangeTo");
					bref.Reset(bid);
				}
			}
		}
}

#define ET_MOB_EXIST_DISTANCE (sqrt(pow2(PL_VIEW_W_HALF)+pow2(PL_VIEW_H_HALF)) * 4.5)
void Entity::CheckDeath()
{
	if(OuttaWorld(x,y) 
	|| IsProjectileET(GetType()) 
	    && (world.GetBlock(x,y,"Entity::CheckDeath").IsBarrier()
		||  GetRStep() <= 0)
	)
	{
		Die(true);
		return;
	}
	if (GetType() == ETT_MONSTER 
		&& Distance(x, y, GetPlayerX(), GetPlayerY()) > ET_MOB_EXIST_DISTANCE)
	{
		Die(false);
		return;
	}
	if (tag.HaveKey("Life"))
	{
		if (clock() - start > tag.GetLong("Life"))
		{
			Die(false);
			return;
		}
	}
}
inline bool IsSuitableEntitySpawn(const long& x, const long& y, const BiomeEntitySpawnData& esdata)
{
	return !OuttaWorld(x, y) && !world.GetBlock(x,y,"IsSuitableEntitySpawn").IsBarrier()
		&& (!esdata.allowInWater || world.GetBlock(x, y, "IsSuitableEntitySpawn").id != EnNameToBlock("water"));
}
void SpawnEntityNearby(ENTITY_ID id, BIOME_ID biome_id, const BiomeEntitySpawnData& esdata)
{
	long x = 0, y = 0;
	long px = p.x, py = p.y;
	do {
		float xk = Choice({ 1.0, -1.0 });
		float yk = Choice({ 1.0,-1.0 });
		x = RandomRange(ESPAWN_MIN_DISTANCE, ESPAWN_MAX_DISTANCE) * xk + px;
		y = RandomRange(ESPAWN_MIN_DISTANCE, ESPAWN_MAX_DISTANCE) * yk + py;
	} while (!IsSuitableEntitySpawn(x,y,esdata));
	//DebugLog("刷怪：X=" + ToString(x) + " Y=" + ToString(y) + " id=" + ToString(id));
	SpawnEntity(id, x, y, RandomDir4());
}
void CheckAchvDigs(BLOCK_ID id)
{
	XCPT_L
	for(ACHV_ID i = 0; i < achievement_data.size(); ++i)
	{
		if(!p.HaveAchieved(i) && achievement_data.at(i).cond.DigThisOK(id))
		{
			p.Achieve(i);
			continue;
		}
	}
	XCPT_R
}
bool AchievementData::PartialCheckIsOK(void) const
{
	if(cond.type == ACDT_GET)
	{
		if (cond.GetThisOK(p.bag.tmpItem))
			return true;
		for(short i = 0; i < ITEM_CNT; ++i)
		{
			if(cond.GetThisOK(p.bag.items[i]))
			    return true;
		}
	}
	return false;
}
void CheckAchvGets()
{
	XCPT_L
	for(ACHV_ID i = 0; i < achievement_data.size(); ++i)
	{
		if(!p.HaveAchieved(i) && achievement_data.at(i).PartialCheckIsOK())
		{
			p.Achieve(i);
			continue;
		}
	}
	XCPT_R
}
void CheckAchvUses(const Item& item)
{
	XCPT_L
	for(ACHV_ID i = 0; i < achievement_data.size(); ++i)
	{
		if(!p.HaveAchieved(i) && achievement_data.at(i).cond.UseThisOK(item))
		{
			p.Achieve(i);
			continue;
		}
	}
	XCPT_R
}

long AttainNewUUID(void)
{
	long uuid=0;
	bool ok=true;
	do{
		uuid = RandomRange(1,LONG_MAX-1);
		for(short j = 0; j < players.size(); ++j)
		    if(players.at(j).uuid == uuid)
		        ok=false;
		for(short i = 0; i < entities.size(); ++i)
		    if(entities.at(i).uuid == uuid)
		        ok=false;
	}while(!ok);
	return uuid;
}

CraftPlace ParseCraftPlace(const string& src, bool updateTotalPlaceCount) {
	XCPT_L
	CraftPlace res;
	if(src.empty())
		return res;
	if(src.find('.') == string::npos) {
		res.kind = src[0];
	} else {
		vector<string> cut = CutLine(src, '.');
		res.kind = cut[0][0];
		if(updateTotalPlaceCount && res.kind-'A'+1 > total_kind_cnt) {
			//Update total count
			total_kind_cnt = res.kind-'A'+1;
		}
		res.level = Convert<string,short>(cut[1]);
	}
	return res;
	XCPT_R
}

void Entity::PlayerInteraction() {
	XCPT_L
	if(GetType() == ETT_ITEM) {
		if(status == EST_DEAD)	return;
		ITEM_ID iid=0;
		short cnt=1;
		JSON itag;
		bool suc = GetItemData(iid, cnt, &itag);
		if(!suc)
			return;
		for(short i = 0; i < players.size(); ++i) {
			if(players.at(i).x == x && players.at(i).y == y && !players.at(i).dead) {
				//拾取
				short remain = players.at(i).GainItem(iid,	cnt, itag.ToString().c_str());
//				DebugLog("remain="+ToString(remain));
				if(remain > 0) {
					cnt = remain;
					tag.SetShort("Count",cnt);
					continue;
				}
				status = EST_DEAD;	//被第一个人拾取
				break;
			}
		}
	}
	else if (GetType() == ETT_BLADE)
	{
		Player* pptr = nullptr;
		if ((pptr = FindPlayer(GetOwnerUUID())) != nullptr
			&& (pptr->x != x || pptr->y != y))
		{	//与玩家坐标同步
			x = pptr->x;
			y = pptr->y;
		}
	}
	XCPT_R
}
bool CraftSystem::CraftOnce(RECIPE_ID rid, Inventory* bag) {
	XCPT_L
	/*if(!IsActiveRecipe(rid,bag))
		return false;*/
	Recipe rec(recipes.at(rid));
	for(short j = 0; j < rec.src.size(); ++j) {
		for(short i = 0; i < ITEM_CNT; ++i) {
			if(rec.src.at(j).cnt <= 0)	//done
				break;
			if(bag->items[i].id == rec.src.at(j).id) {
				//single slot match
				if(bag->items[i].cnt >= rec.src.at(j).cnt) {
					bag->items[i].Lose(rec.src.at(j).cnt);
					rec.src.at(j).cnt = 0;	//this material is ok
					break;
				} else {	//this slot's not enough
					rec.src.at(j).cnt -= bag->items[i].cnt;
					bag->items[i].Lose(bag->items[i].cnt);
				}
			}
		}
	}
	if(bag->tmpItem.id == 0) {
		bag->tmpItem.Reset(rec.dst.id, rec.dst.cnt/*, rec.dst.tag*/);
	} else if(bag->tmpItem.id == rec.dst.id && !bag->tmpItem.IsFull()) {
		if(bag->tmpItem.GetMaxHeap() - bag->tmpItem.cnt >= rec.dst.cnt)	//add
			bag->tmpItem.cnt += rec.dst.cnt;
		else {	//both
//			MsgSndTip();
			p.GainItem(rec.dst.id, bag->tmpItem.cnt + rec.dst.cnt - bag->tmpItem.GetMaxHeap()/*, rec.dst.tag.ToString().c_str()*/) ;
			bag->tmpItem.cnt = bag->tmpItem.GetMaxHeap();
		}
	} else {	//gain
		p.GainItem(rec.dst.id, rec.dst.cnt/*, rec.dst.tag.ToString().c_str()*/) ;
	}
	XCPT_R
	return true;
}

//#define PLANT_SHAKE_TIME 5000
#define PLANT_SHAKE_AMP (g_block_px*0.1)
#define PLANT_SHAKE_TAU 3000.0
#define PLANT_SHAKE_OMEGA 0.003
#define PLANT_SHAKE_PHASE (Choice({PI/2.0,PI/4.0}))
#define SIDEFX_BLUR_VALUE 254
#define SIDEFX_ALPHA_VALUE ((p.HaveEffect(2)||world.IsNight())?128:235)
void DrawBlock(Block& b, int _x, int _y, const int& bx, const int& by, bool sideFX) {
	XCPT_L
	if(b.id == 0 || !b.shape)	return;
	if (sideFX && !options.view_side_fx)
		sideFX = false;
	PIMAGE img_dest = nullptr;
	if (sideFX)
	{
		img_dest = newimage(g_block_px, g_block_px);
	}

	setfont(g_block_px,0,"Determination Mono");
	color_t color = b.color;
	if(b.light < NORMAL_LIGHT_LVL)
//		color = StepColor(color, EGERGB(10,10,10), (NORMAL_LIGHT_LVL - b.light)/float(NORMAL_LIGHT_LVL-1));
		color = LightnessBlockColor(color, b.light);
	else{
		//<!> hilite...
	}
	if(!b.HaveClass(BCLASS_LIQUID))
//		setcolor(StepColor(DARKGRAY, color , ((double)b.dur / (blockdata.at(b.id).dur))));
//		setcolor(LightnessBlockColor(color,(b.dur % 2 == blockdata.at(b.id).dur % 2 ? NORMAL_LIGHT_LVL : 1)));
		setcolor(LightnessBlockColor(color, 4 + (NORMAL_LIGHT_LVL - 4) * ((double)b.dur / (blockdata.at(b.id).dur))));
	else
		setcolor(color);
	//pxprint(_x, _y, b.shape);
	if ((b.HaveClass(BCLASS_TREE) || b.HaveClass(BCLASS_PASSABLE)) /*&& clock() - world.lastPlantShake <= PLANT_SHAKE_TIME*/
		&& (bx - p.x + by - p.y) % 3 == 0)
	{
		double _t = (clock() - world.lastPlantShake - 50 * (bx - p.x));
		int x_offset = PLANT_SHAKE_AMP * exp(-_t / PLANT_SHAKE_TAU) * sin(PLANT_SHAKE_OMEGA * _t);
		int y_offset = PLANT_SHAKE_AMP * exp(-_t / PLANT_SHAKE_TAU) * sin(PLANT_SHAKE_PHASE + PLANT_SHAKE_OMEGA * _t);
		//int x_offset = Choice({ -0.15,-0.12,-0.1,-0.1,0.0,0.1,0.1,0.12,0.15 }) * g_block_px;
		//int y_offset = Choice({ -0.15,-0.12,-0.1,-0.1,0.0,0.1,0.1,0.12,0.15 }) * g_block_px;
		ClampA<int>(x_offset, -g_block_px * 0.2, g_block_px * 0.2);
		ClampA<int>(y_offset, -g_block_px * 0.2, g_block_px * 0.2);
		_x += x_offset;
		_y += y_offset;
	}
	if (b.shape != nullptr && string(b.shape)[0] != 'U')
	{
		pxprint((sideFX?0:_x), (sideFX ? 0 : _y), b.shape, 0, 0, img_dest);
	}
	else {
		//MessageBoxA(NULL,ToString(HexToDec(strxhead(b.shape, 1))).c_str(), b.shape,MB_OK);
		pxprint((sideFX ? 0 : _x), (sideFX ? 0 : _y), HexToDec(strxhead(b.shape, 1)), 0, 0, img_dest);
	}
	if (world.wpid == WPID_RAINDROP && b.HaveClass(BCLASS_LIQUID))
	{
		if (RandomRange(0, 100, true, false) < 2)
		{	//水花
			setcolor(StepColor(color, (world.IsDay()?EGERGB(255, 255, 255):EGERGB(0,0,0)), 0.4));
			pxprint((sideFX ? 0 : _x), (sideFX ? 0 : _y), Choice({ 0x02EF, 0x03B3, 0x03BD, 0x03D2, 0x0475, 0x0582, 0x055D, 0x05C3, 0x0846}), 0, 0, img_dest);
		}
	}
	if (sideFX)
	{
		imagefilter_blurring(img_dest, SIDEFX_BLUR_VALUE, SIDEFX_ALPHA_VALUE);
		//putimage_withalpha(NULL, img_dest, _x, _y);
		putimage(_x, _y, img_dest, SRCCOPY);
		delimage(img_dest);
		img_dest = nullptr;
	}
	XCPT_R
}
bool OuttaWorld(const long& _x, const long& _y, bool onlyLoaded/*=true existing*/) {
	if (onlyLoaded)
		return world.manager.OuttaLoadedWorld(_x, _y);
	if(_x < 0 || _y < 0
	        || _x >= world.manager.map_w || _y >= world.manager.map_h)
		return true;
	return false;
}
#define TOUCH_BLOCK_SHAKE_TIME 220
#define TOUCH_BLOCK_SHAKE_AMP 11
#define TOUCH_BLOCK_SHAKE_OMEGA 0.05
#define TOUCH_BLOCK_SHAKE_PHASE Choice({PI/2.0,PI/4.0,PI*0.75})
#define TOUCH_BLOCK_SHAKE_TAU 60.0
void DrawSceneBlocks() {
	XCPT_L

	for(long rel_y=-PL_VIEW_H_HALF; rel_y <= PL_VIEW_H_HALF; ++rel_y)
		for(long rel_x=-PL_VIEW_W_HALF; rel_x <= PL_VIEW_W_HALF; ++rel_x) {
			long abs_x = GetPlayerX() + rel_x;
			long abs_y = GetPlayerY() + rel_y;
			if(OuttaWorld(abs_x,abs_y)
			        || world.GetBlock(abs_x,abs_y,"DrawSceneBlocks").HaveClass(BCLASS_AIR)
			  )
				continue;	//跳过
			bool onViewSide = (abs(rel_x) == PL_VIEW_W_HALF || abs(rel_y) == PL_VIEW_H_HALF);
			Block& bref = world.GetBlock(abs_x,abs_y,"DrawSceneBlocks");
//            DebugLog(ToString(bref.id)+" "+ToString(abs_x)+","+ToString(abs_y));
			int scr_x=0;
			int scr_y=0;
			scr_x = GetPlayerDrawLeft() + (abs_x - GetPlayerX()) * g_block_px;
			scr_y = GetPlayerDrawTop() + (abs_y - GetPlayerY()) * g_block_px;

			if (p.lastPutOrDigBlock_coord.X == abs_x && p.lastPutOrDigBlock_coord.Y == abs_y
				|| p.lastOperateBlock_coord.X == abs_x && p.lastOperateBlock_coord.Y == abs_y)
			{
				double _x = clock() - (p.lastPutOrDigBlock_coord.X == abs_x && p.lastPutOrDigBlock_coord.Y == abs_y ? p.lastAttack : p.lastOperate);
				int x_offset = TOUCH_BLOCK_SHAKE_AMP * exp(-_x / TOUCH_BLOCK_SHAKE_TAU) * sin(TOUCH_BLOCK_SHAKE_OMEGA * _x);
				int y_offset = TOUCH_BLOCK_SHAKE_AMP * exp(-_x / TOUCH_BLOCK_SHAKE_TAU) * sin(TOUCH_BLOCK_SHAKE_PHASE + TOUCH_BLOCK_SHAKE_OMEGA * _x);
				ClampA(x_offset, -80, 80);
				ClampA(y_offset, -80, 80);
				scr_x += x_offset;
				scr_y += y_offset;
			}
			DrawBlock(bref, scr_x,scr_y,abs_x,abs_y, onViewSide);
		}

	XCPT_R
}
void DrawDebugInfo();

void DrawPlayer() {
	//输出玩家
	setfont(g_block_px,0,"Determination Mono");
	color_t color = GetPlayerColor();
	if (p.dead)
		color = RED;
	else if(p.IsBeingHurt())
		color = LIGHTRED;
	setcolor(LightnessBlockColor(color, OuttaWorld(p.x,p.y)? NORMAL_LIGHT_LVL :world.GetBlock(p.x,p.y,"DrawPlayer").light));
	midpxprint(GetPlayerShapeID(),GetPlayerDrawTop(),scene_left,scene_right,0);
}
#define hotbar_left rui_left+ITEM_FS*2
#define hotbar_top inven_top+ITEM_FS
#define exten_w (200*(uix / 1920.0))
#define inven_hotbar_left rui_left - exten_w
#define inven_hotbar_top hotbar_top
#define inven_hotbar_gap (20*(uiy/1080.0))
void DrawInventory() {
	setfont(ITEM_FS,0,"Determination Mono");
	setcolor(CYAN);
	midpxprint("—————————",inven_top,
	            (p.bag.isOpen?inven_hotbar_left:rui_left),
	            rui_right - (p.bag.isOpen?exten_w:0));
	if(!p.bag.isOpen)
		p.bag.DrawHotbar(hotbar_left,hotbar_top);
	else {
		p.bag.DrawInventory(inven_hotbar_left+inven_hotbar_gap,inven_hotbar_top);
	}
}
void DrawPlayerBars(int _left, int _top) {
	setwcolor(LIGHTRED);
	pxprint(_left, _top, "生命");
	setwcolor(YELLOW);
	char s[20];
	sprintf(s, "%3d/%3d",p.hp, p.max_hp);
	if(p.IsBeingHurt())
	{
		setfillcolor(DARKGRAY);
		bar(_left+ITEM_FS*3 - 20,_top,_left+ITEM_FS*8,_top+ITEM_FS);
	}
	pxprint(_left+ITEM_FS*3,_top, s);

	setwcolor(EGERGB(228,129,22));
	pxprint(_left, _top + ITEM_FS, "饱腹");
	setwcolor(LIGHTGRAY);
	if(p.sp > p.max_sp)
		sprintf(s, "%3d+/%3d",p.max_sp, p.max_sp);	//over_saturated
	else
		sprintf(s, "%3d/%3d",p.sp, p.max_sp);
	pxprint(_left+ITEM_FS*3,_top+ITEM_FS, s);

	setwcolor(EGERGB(72,112,240));
	pxprint(_left, _top + ITEM_FS*2, "魔法");
	setwcolor(LIGHTCYAN);
	sprintf(s, "%3d/%3d",p.mp, p.max_mp);
	pxprint(_left+ITEM_FS*3,_top+ITEM_FS*2, s);
}
void DrawRightUI() {
	static int rui_text_fs = ITEM_FS;
	setcolor(LIGHTGRAY);
	setlinestyle(PS_SOLID,0,3);
	line(rui_left-(p.bag.isOpen?exten_w:0),rui_text_top*0.7,rui_left-(p.bag.isOpen?exten_w:0),uiy-100);
	setfillcolor(BLACK);
	if(!p.bag.isOpen)
		bar(rui_left,0,uix,uiy);
	else
		bar(rui_left-exten_w,0,uix,uiy);

	setfont(rui_text_fs,0,"Determination Mono");
	setwcolor(WHITE);
	midpxprint(GetPlayerName(),rui_text_top,rui_left-(p.bag.isOpen?exten_w:0),rui_right-(p.bag.isOpen?exten_w:0));
	setwcolor(YELLOW);
	midpxprint("X="+ToString(GetPlayerX())+" Y="+ToString(GetPlayerY()),rui_text_top+rui_text_fs,rui_left-(p.bag.isOpen?exten_w:0),rui_right-(p.bag.isOpen?exten_w:0));
	setwcolor(DARKGRAY);
	midpxprint(world.GetTime(),uiy-rui_text_fs,0,rui_left-(p.bag.isOpen?exten_w:0));

	DrawPlayerBars(rui_left+50-(p.bag.isOpen?exten_w:0),rui_text_top+rui_text_fs*2);
	DrawInventory();
}
inline bool IsBarrier(Block& bk) {
	return bk.IsBarrier();
}

bool OuttaWorldDir(long x, long y, DIR dir, bool onlyLoaded = true) {
	DirOffsetPos(x,y,dir);
	return OuttaWorld(x, y, onlyLoaded);
}
bool IsBarrier(long ox, long oy, DIR dir) {
	if(OuttaWorldDir(ox,oy,dir))	return true;
	DirOffsetPos(ox,oy,dir);
	return IsBarrier(world.GetBlock(ox,oy,"IsBarrier"));
}
Player* FacingPlayer(long ox, long oy, DIR dir)
{
	if(OuttaWorldDir(ox,oy,dir))	return nullptr;
	if(players.empty())	return nullptr;
	DirOffsetPos(ox,oy,dir);
	for(short j = 0; j < players.size(); ++j)
		if(players.at(j).x == ox && players.at(j).y == oy)
		{
			return &players.at(j);
		}
	return nullptr;
}
Player* FrontPlayer(long ox, long oy, DIR dir)
{
	if(players.empty())	return nullptr;
	for(short j = 0; j < players.size(); ++j)
	{
		if(players.at(j).x == ox && players.at(j).y == oy
		|| dir == RIGHT && players.at(j).y == oy && players.at(j).x > ox
		|| dir == LEFT && players.at(j).y == oy && players.at(j).x < ox
		|| dir == UP && players.at(j).x == ox && players.at(j).y < oy
		|| dir == DOWN && players.at(j).x == ox && players.at(j).y > oy
		|| dir == LEFTUP && (players.at(j).x - ox) / (players.at(j).y - oy) == 1 && players.at(j).x <= ox
		|| dir == RIGHTDOWN && (players.at(j).x - ox) / (players.at(j).y - oy) == 1 && players.at(j).x >= ox
		|| dir == LEFTDOWN && (players.at(j).x - ox) / (players.at(j).y - oy) == -1 && players.at(j).x <= ox
		|| dir == RIGHTUP && (players.at(j).x - ox) / (players.at(j).y - oy) == -1 && players.at(j).x >= ox
		)
			return &players.at(j);
	}
	return nullptr;
}
Entity* FacingEntity(long ox, long oy, DIR dir, bool onlyMonsterOrAnimal=false)
{
	if(OuttaWorldDir(ox,oy,dir))	return nullptr;
	if(entities.empty())	return nullptr;
	DirOffsetPos(ox,oy,dir);
	for(short j = 0; j < entities.size(); ++j)
		if(entities.at(j).x == ox && entities.at(j).y == oy 
			&& (!onlyMonsterOrAnimal || entities.at(j).GetType() == ETT_MONSTER || entities.at(j).GetType() == ETT_ANIMAL
				)
			&& (!entitydata.at(entities.at(j).id).data.HaveKey("NotBarrier")
			|| entitydata.at(entities.at(j).id).data.GetShort("NotBarrier")==0))
		{
			return &entities.at(j);
		}
	return nullptr;
}
Entity* ExistEntityHere(long x, long y, bool onlyCreature = false, const vector<long>* except_uuids = nullptr)
{
	if (entities.empty() || OuttaWorld(x, y))	return false;
	for (short j = 0; j < entities.size(); ++j)
		if (entities.at(j).x == x && entities.at(j).y == y
			&& (!onlyCreature || IsCreature(entities.at(j).GetType()))
			)
		{
			if (except_uuids == nullptr || !Contain(*except_uuids, entities.at(j).uuid))
				return &entities.at(j);
			else
				continue;
		}
	return nullptr;
}
Player* ExistPlayerHere(long x, long y, const vector<long>* except_uuids = nullptr)
{
	if (players.empty() || OuttaWorld(x, y))	return false;
	for (short j = 0; j < players.size(); ++j)
		if (players.at(j).x == x && players.at(j).y == y
			&& (except_uuids == nullptr || !Contain(*except_uuids, players.at(j).uuid)))
			return &players.at(j);
	return nullptr;
}
#define IDLE_MOVE_RATE 30
void Entity::Move()
{	//Entit y::Move
	XCPT_L
	if (ai == AI_STILL || OuttaWorld(x,y))	return;
	if (GetType() != ETT_ITEM && ai == AI_IDLE)
	{
		if (clock() - lastIdleRefresh > 1000)
		{
			lastIdleRefresh = clock();
			int r = RandomRange(0, 100, true, false);
			if (r <= IDLE_MOVE_RATE)
			{
				dir = RandomDir4();
				long _x = x;
				long _y = y;
				DirOffsetPos(_x, _y, dir, "Entity::Move");
				if (!OuttaWorld(_x, _y) && !world.GetBlock(_x, _y, "Entity::Move").IsBarrier())
				{
					x = _x, y = _y;
					if (TimeToAttack())
						lastAttack = (clock() - GetAttackCD()/2);
					lastMove = clock();
				}
			}
		}
		return;
	}
	if(GetType() == ETT_ITEM || !TimeToMove())
			return;
	if(ai == AI_LINE)
	{
		DirOffsetPos(x,y,dir,"Entity::Move");
		short rstep = GetRStep();
		SetRStep(--rstep);
		lastMove=clock();
	}else if(ai == AI_CHASE)
	{
		DIR the_dir = GetChasingDir(x,y, GetPlayerX(), GetPlayerY());
		dir = the_dir;
		if(!IsBarrier(x,y,the_dir) && !FacingPlayer(x,y,the_dir) && !FacingEntity(x,y,the_dir,true))
		{
			DirOffsetPos(x,y,the_dir,"Entity::Move");
		}
		if (TimeToAttack())
			lastAttack = (clock() - GetAttackCD() * 1.4);
		lastMove=clock();
	}else if(ai == AI_FLEE)
	{
		DIR the_dir = GetFleeingDir(x,y, GetPlayerX(), GetPlayerY());
		dir = the_dir;
		if(!IsBarrier(x,y,the_dir) && !FacingPlayer(x,y,the_dir) && !FacingEntity(x,y,the_dir,true))
		{
			DirOffsetPos(x,y,the_dir,"Entity::Move");
		}
		lastMove=clock();
	}
	else{
		ErrorLogTip(NULL,"无效的AI类型="+ToString(ai),"Entity::Move");
	}
	XCPT_R
}
// mean:期望， stddev:标准差
double Gaussian_noise(double mean, double stddev) {
	unsigned seed = chrono::system_clock::now().time_since_epoch().count();
	default_random_engine generator(seed);
	normal_distribution<double> dist(mean, stddev);
	return dist(generator);
}
//from https ://blog.csdn.net/weixin_42099090/article/details/116484743
#define FLD_STDDEV(d) (1/24.0*d)
inline void FluctuateDamage(short& dmg, long owner_uuid)
{	//伤害的正态波动
	Player* pptr = FindPlayer(owner_uuid);
	if (pptr != nullptr)
	{
		dmg = Gaussian_noise(double(dmg*Clamp(fabs(1.0 - pptr->luck / 50.0)*0.5 + 1.0, 0.65, 1.35)), FLD_STDDEV(dmg));
	}
	else
	{
		dmg = Gaussian_noise((double)dmg, FLD_STDDEV(dmg));
	}
		
}
void ExtraAction(const JSON& from, Creature* target)
{
	if (from.HaveKey("ExtraEffects"))
	{	//额外效果
		vector<string> sfx = from.GetList("ExtraEffects");
		for (const auto& seff : sfx)
		{
			vector<string> cut = CutLine(strxhead(strxtail(seff)), '*');
			EFFECT_ID effid = Convert<string, USHORT>(UncoverValue(cut.at(0), "short"));
			DWORD duration = Convert<string, DWORD>(UncoverValue(cut.at(1), "long"));
			if (cut.size() > 2)
			{
				float chance = Convert<string, float>(UncoverValue(cut.at(2), "float"));
				if (!Percent(chance*100.0)) goto _2;
			}
			//AddChatItemPlain("effid=" + ToString(effid) + " duration=" + ToString(duration));
			target->AddEffect(effid, duration);
		}
	}
	if (from.HaveKey("ClearEffects"))
	{
		vector<string> sfx = from.GetList("ClearEffects");
		for (const auto& seff : sfx)
		{
			EFFECT_ID effid = Convert<string, USHORT>(UncoverValue(seff, "short"));
			target->ClearEffect(effid);
		}
	}
_2:;
}
void ExtraAttack(ENTITY_ID eid, Creature* cptr)
{
	if (eid == 0)	return;
	ExtraAction(entitydata.at(eid).data, cptr);
}
void ApproachKnockback(KBLEVEL kblvl, DIR knockDir, Creature* cptr)
{	//被击退
	if (cptr == nullptr || kblvl == 0)	return;
	AddChatItemPlain("x=" + ToString(cptr->x) + " y=" + ToString(cptr->y));
	auto [add_clock, kbDist] = CalcKnockback(kblvl, cptr->x, cptr->y, knockDir);
	AddChatItemPlain("add_clock=" + ToString(add_clock) + " kbDist=" + ToString(kbDist));
	for (long _ = 0; _ < kbDist; ++_)
		DirOffsetPos(cptr->x, cptr->y, knockDir, "ApproachKnockback");
	cptr->lastMove += add_clock;
	cptr->lastAttack += add_clock;
}
long PointAttack(Entity* whoptr, long x, long y, short att, long ownerUUID, const vector<long>* except)
{	//one point is only able to attack one entity or player
	if (OuttaWorld(x, y))	return INVALID_UUID;
	//DebugLog(ToString(except->size()));
	if (Entity* eptr = nullptr; 
		(eptr = ExistEntityHere(x, y, true, except)) != nullptr
		&& eptr->uuid != ownerUUID)
	{	//Strike an entity
		//DebugLog("Struck uuid=" + ToString(eptr->uuid));
		FluctuateDamage(att, ownerUUID);	//HERE FLUCTUATE
		if (att < 1)	att = 1;
		eptr->Hurt(att);
		ExtraAttack(whoptr->id, (Creature*)eptr);
		ApproachKnockback(whoptr->GetKnockbackLevel(), whoptr->dir, (Creature*)eptr);
		return eptr->uuid;
	}
	else if (Player* pptr = nullptr;
		(pptr = ExistPlayerHere(x, y, except)) != nullptr
		&&pptr->uuid != ownerUUID)
	{	//Strike a player
		FluctuateDamage(att, ownerUUID);	//HERE FLUCTUATE
		if (att < 1)	att = 1;
		pptr->Hurt(att);
		ExtraAttack(whoptr->id, (Creature*)pptr);
		ApproachKnockback(whoptr->GetKnockbackLevel(), whoptr->dir, (Creature*)eptr);
		return pptr->uuid;
	}
	return INVALID_UUID;
}
void Entity::Attack()
{	//Entit y::Attack
	XCPT_L
	if(IsProjectileET(GetType()))
	{	//投掷物 
		long owner = GetOwnerUUID();
		short att = GetAttack();
		FluctuateDamage(att, owner);
		if (att < 1)	att = 1;
		for(short i = 0; i < players.size(); ++i)	//Players check
		{
			if(players.at(i).x == x && players.at(i).y == y //same coordination
			&& players.at(i).uuid != owner)					//not owner
			{
				players.at(i).Hurt(att);
				ExtraAttack(id, (Creature*)&players.at(i));
				ApproachKnockback(GetKnockbackLevel(), dir, (Creature*)&players.at(i));
				Die();
				return;
			}
		}
		for(short e = 0; e < entities.size(); ++e)	//Entities check
		{
			if(
			entities.at(e).uuid != uuid	//not itself
			&& (entities.at(e).GetType() == ETT_MONSTER	|| entities.at(e).GetType() == ETT_ANIMAL)
			&& entities.at(e).x == x && entities.at(e).y == y	//same coordination
			&& entities.at(e).uuid != owner
			)	//not owner
			{
				entities.at(e).Hurt(att);
				ExtraAttack(id, (Creature*)&entities.at(e));
				ApproachKnockback(GetKnockbackLevel(), dir, (Creature*)&entities.at(e));
				Die();
				return;
			}
		}
	}else if(GetType() == ETT_MONSTER || GetType() == ETT_ANIMAL)
	{
		if(!TimeToAttack())
			return;
		Player* ptr=nullptr;
		
		if(CanCloseAttack())
		{
			if(ptr=FacingPlayer(x,y,dir))
			{
				short dmg = GetAttack();
				FluctuateDamage(dmg, this->uuid);
				ptr->Hurt(dmg);
				ExtraAttack(id, (Creature*)ptr);
				ApproachKnockback(GetKnockbackLevel(), dir, (Creature*)ptr);
				lastAttack=clock();
			}
		}
		if(CanFarAttack())
		{
			if(ptr = FrontPlayer(x,y,dir))
			{
				short dmg = GetFarAttack();
				ENTITY_ID projID = GetProjID();
				long _x=x,_y=y;
				short move_cd = GetProjMoveCD();
				short range = GetRange();
				KBLEVEL kblvl = GetKnockbackLevel();
				DirOffsetPos(_x,_y,dir,"Entity::Attack");
				SpawnProjectile(projID,_x,_y,dir,uuid,dmg,move_cd,range,kblvl);
				lastAttack=clock();
			}
		}
	}
	else if (GetType() == ETT_BLADE)
	{
		vector<long> victims;
		if (!tag.HaveKey("Victims"))
			tag.AddItem("Victims", "[]");
		else
		{
			auto svictims = tag.GetList("Victims");
			for (const auto& svictim : svictims)
				victims.push_back(Convert < string,long > (UncoverValue(svictim, "long")));
		}
		vector<POINT> dps = GetDamagePoints(GetDPID());
		DamagePointsRotate(dps, dir);
		for (const auto& dp : dps)
		{
			long abs_x = p.x + dp.x;
			long abs_y = p.y + dp.y;
			long hit_uuid = PointAttack(this, abs_x, abs_y, GetAttack(), GetOwnerUUID(), &victims);
			if (hit_uuid != INVALID_UUID)
			{
				p.OnBladeHit(tag.GetShort("FromItemID"));
				victims.push_back(hit_uuid);	//赦免已经打击过的受害者
			}
		}
		vector<string> svictims;
		for (const auto& victim : victims)
			svictims.push_back(ToString(victim) + "L");
		tag.SetList("Victims", svictims);
	}
	XCPT_R
}

#define WHEEL_CD 30	//ms
bool ms_msg_trg=false;
clock_t lastWheel;
mouse_msg ms_msg;
void CraftSystem::RefreshPrecur(void) {
	XCPT_L
	if(ms_msg.x > CSYS_LEFTBAR_W || ms_msg.y < CSYS_ITEM_TOP) {
		uid craft_precur = -1;
		return;
	}
	uid craft_precur = (ms_msg.y - CSYS_ITEM_TOP) / CSYS_ITEM_H;
	if(uid craft_precur >= CSYS_ITEM_CNT) {
		uid craft_precur = -1;
		return;
	}
	XCPT_R
}
void RefreshBlockOperationTip() {
	//Update the preselected block operation tip
	XCPT_L
	if(p.preblock == nullptr)
		return;
	float _dist = Distance(p.preblock_coord.X, p.preblock_coord.Y, p.x, p.y);
	bool canReach = p.CanReach(_dist);
	if(!canReach) {
		uid block_op_tip = "";
		return;
	}

	if(p.preblock->HaveClass(BCLASS_DOOR)) {
		short isOpen=0;
		if (!p.preblock->tag.HaveKey("Open")) {
			//abnormal
			p.preblock->tag.AddItem("Open", "0s");
		}
		bool suc = p.preblock->tag.GetSimpleValue<short>("Open",&isOpen);
		if(!suc)
			uid block_op_tip = "方块数据有误!";
		else if(isOpen)
			uid block_op_tip = "关上";
		else
			uid block_op_tip = "打开";
	} else if(p.preblock->HaveClass(BCLASS_CONTAINER)) {
		uid block_op_tip = "查看";
	} else if(p.preblock->HaveClass(BCLASS_BURNER) && p.bag.HandItemCRef().IsFuel()){
		uid block_op_tip = "添燃料";
	} else if(p.preblock->HaveClass(BCLASS_CRAFT)) {
		uid block_op_tip = "合成";
	} else if(p.preblock->HaveClass(BCLASS_FRUITFUL) && p.preblock->HaveFruit()) {
		uid block_op_tip = "收获";
	} else if(p.preblock->HaveClass(BCLASS_PICKUP)) {
		uid block_op_tip = "拾起";
	}else
		uid block_op_tip = "";
	XCPT_R
}
void RefreshPreblock(void) {
	//Update the preselected block in the view
	long rel_x=0,rel_y=0;
	long abs_x=0,abs_y=0;
	if(ms_msg.x > GetPlayerDrawLeft())
		rel_x = (ms_msg.x - GetPlayerDrawLeft())/g_block_px;
	else
		rel_x = (ms_msg.x - GetPlayerDrawLeft() - g_block_px)/g_block_px;
	if(ms_msg.y > GetPlayerDrawTop())
		rel_y = (ms_msg.y - GetPlayerDrawTop())/g_block_px;
	else
		rel_y = (ms_msg.y - GetPlayerDrawTop() - g_block_px)/g_block_px;
	if(abs(rel_x) > PL_VIEW_W_HALF || abs(rel_y) > PL_VIEW_H_HALF
	        || (p.bag.isOpen && (ms_msg.x >= rui_left-exten_w || ms_msg.x <= CSYS_LEFTBAR_W ))) {
		//Out of view or On the UI
		p.preblock = nullptr;
		return;
	}
	abs_x = rel_x + p.x;
	abs_y = rel_y + p.y;
	if(OuttaWorld(abs_x,abs_y)) {
		//Invalid World Coord
		p.preblock = nullptr;
		return;
	}
	p.preblock_coord.X = abs_x;
	p.preblock_coord.Y = abs_y;
	//Pointer pointing
	p.preblock = &world.GetBlock(abs_x,abs_y,"RefreshPreblock");

	RefreshBlockOperationTip();
}
vector<string> cur_itemtag;
void RefreshContainerPrecur()
{
	XCPT_L
	if(ms_msg.x >= world.insp_ctn_left && ms_msg.y >= world.insp_ctn_top
	&& ms_msg.x <= insp_ctn_right && ms_msg.y <= insp_ctn_bottom)
	{
		short _x=0,_y=0;
		_x = (ms_msg.x - world.insp_ctn_left) / CONTAINER_SLOT_W;
		_y = (ms_msg.y - world.insp_ctn_top) / CONTAINER_ITEM_FS;
		world.insp_precur = _x * world.insp_h + _y;
		if(world.insp_precur < 0 || world.insp_precur >= world.insp_container.size())
			world.insp_precur = -1; 
	}else
		world.insp_precur = -1;
	XCPT_R
}
void RefreshMouseTips() {
	XCPT_L

	int mx,my;
	GetMousePos(mx,my);
	if(p.bag.isOpen) {
		p.craft.RefreshPrecur();
		p.bag.RefreshPrecur(inven_hotbar_left+inven_hotbar_gap, hotbar_top, mx, my);
	} else
		p.bag.RefreshPrecur(hotbar_left, hotbar_top, mx, my);
	if(p.inspblock != nullptr)
	{	//查看中 
		RefreshContainerPrecur();
	}

	if(p.bag.precur != -1	//wanna select inventory item
	        && K(VK_LBUTTON)) {
		if(!p.bag.isOpen) {
			if (p.bag.cur != p.bag.precur)
				p.OnItemSwitch(p.bag.cur, p.bag.precur);
			p.bag.cur = p.bag.precur;	//SELECT
		} else if(p.bag.precur < ITEM_CNT) {	//precur valid range
			if((!K(VK_SHIFT) || p.bag.tmpItem.id != 0 || p.inspblock==nullptr) 
			&& clock() - p.bag.lastSwap > SWAP_CD)
			{
				if(p.bag.items[p.bag.precur].id != 0 
				&& p.bag.items[p.bag.precur].id == p.bag.tmpItem.id
	//			&&	//<!>JSON相同 
				&& !p.bag.items[p.bag.precur].IsFull()
				 )
				{	//merge
					p.bag.items[p.bag.precur].cnt += p.bag.tmpItem.cnt;
					if(p.bag.items[p.bag.precur].cnt > p.bag.items[p.bag.precur].GetMaxHeap())
					{	//Full
						p.bag.tmpItem.cnt = p.bag.items[p.bag.precur].cnt - p.bag.items[p.bag.precur].GetMaxHeap();
						p.bag.items[p.bag.precur].cnt = p.bag.items[p.bag.precur].GetMaxHeap();
						p.bag.tmpItem.Normalize();	//final insurance
					}else{
						//isn't full -> clear tmpItem
						p.bag.tmpItem.Clear();
					}
					SND("obtain");
					p.bag.lastSwap = clock();
				}else{
					if(p.bag.items[p.bag.precur].id != 0)
						p.bag.tmpItemFrom = p.bag.precur;
					SND("obtain");
					if(p.bag.precur == p.bag.cur)
						p.OnItemSwitch(p.bag.cur, p.bag.precur);
					p.bag.items[p.bag.precur].SwapWith(p.bag.tmpItem, p.bag.lastSwap,true);
				}
			} else if(K(VK_SHIFT) && p.bag.items[p.bag.precur].id != 0 )
			{
				if (p.bag.precur == p.bag.cur)
					p.OnItemSwitch(p.bag.cur, p.bag.precur);
				short remain = world.ContainerGainItem(  p.bag.items[p.bag.precur].id,
											p.bag.items[p.bag.precur].cnt,
											p.bag.items[p.bag.precur].tag.ToString().c_str());
				p.bag.items[p.bag.precur].Lose(p.bag.items[p.bag.precur].cnt - remain);
				p.bag.lastSwap = clock();
				SND("obtain");
			}
			
		}
	}else if(p.bag.precur != -1	
	        && K(VK_RBUTTON))
	{	//wanna take part of inventory item
		if(clock() - p.bag.lastSwap > SWAP_CD)
		{
			if (p.bag.precur == p.bag.cur)
				p.OnItemSwitch(p.bag.cur, p.bag.precur);
			if(p.bag.items[p.bag.precur].id != 0 && p.bag.tmpItem.id == 0 && p.bag.items[p.bag.precur].cnt > 1)
			{	//若tmpItem为空，则意为取一半 
				p.bag.tmpItem.id = p.bag.items[p.bag.precur].id;
				p.bag.tmpItem.Gain(ceil(p.bag.items[p.bag.precur].cnt / 2.0),nullptr);
				p.bag.items[p.bag.precur].Lose(p.bag.tmpItem.cnt);
			}else if(p.bag.tmpItem.id == p.bag.items[p.bag.precur].id && !p.bag.items[p.bag.precur].IsFull())
			{	//若tmpItem为同类物品，则意为再放一个 
				p.bag.tmpItem.Lose(1);
				p.bag.items[p.bag.precur].Gain(1,nullptr);
			}else if(p.bag.items[p.bag.precur].id == 0 && p.bag.tmpItem.id != 0)
			{	//若格子为空，则意为放下一个 
				p.bag.tmpItem.Lose(1);
				p.bag.items[p.bag.precur].id = 	p.bag.tmpItem.id;
				p.bag.items[p.bag.precur].Gain(1,nullptr);
			}
			p.bag.lastSwap = clock();
			SND("obtain");
		}
	} else if(p.inspblock != nullptr && world.insp_precur != -1 
	&& clock() - p.bag.lastSwap > SWAP_CD)
	{	//容器物品操作 
		if(K(VK_LBUTTON))
		{
			if(!K(VK_SHIFT) || p.bag.tmpItem.id != 0)
			{
				if(world.insp_container[world.insp_precur].id != 0 
				&& world.insp_container[world.insp_precur].id == p.bag.tmpItem.id
	//			&&	//<!>JSON相同 
				&& !world.insp_container[world.insp_precur].IsFull()
				 )
				{	//merge
					world.insp_container[world.insp_precur].cnt += p.bag.tmpItem.cnt;
					if(world.insp_container[world.insp_precur].cnt > world.insp_container[world.insp_precur].GetMaxHeap())
					{	//Full
						p.bag.tmpItem.cnt = world.insp_container[world.insp_precur].cnt + p.bag.tmpItem.cnt - world.insp_container[world.insp_precur].GetMaxHeap();
						world.insp_container[world.insp_precur].cnt = world.insp_container[world.insp_precur].GetMaxHeap();
					}else{
						//isn't full -> clear tmpItem
						p.bag.tmpItem.Clear();
					}
					p.bag.lastSwap = clock();
				}else{
					/*if(p.bag.items[world.insp_precur].id != 0)
						p.bag.tmpItemFrom = world.insp_precur;*/
					world.insp_container[world.insp_precur].SwapWith(p.bag.tmpItem, p.bag.lastSwap,true);
				}
				SND("obtain");
			}else if(K(VK_SHIFT) && world.insp_container[world.insp_precur].id != 0 ){
				short remain = p.GainItem(  world.insp_container[world.insp_precur].id,
											world.insp_container[world.insp_precur].cnt,
											world.insp_container[world.insp_precur].tag.ToString().c_str());
				world.insp_container[world.insp_precur].Lose(world.insp_container[world.insp_precur].cnt - remain);
				p.bag.lastSwap = clock();
				SND("obtain");
			}
		}else if(K(VK_RBUTTON))
		{
			if(world.insp_container[world.insp_precur].id != 0 && p.bag.tmpItem.id == 0 && world.insp_container[world.insp_precur].cnt > 1)
			{	//若tmpItem为空，则意为取一半 
				p.bag.tmpItem.id = world.insp_container[world.insp_precur].id;
				p.bag.tmpItem.Gain(ceil(world.insp_container[world.insp_precur].cnt / 2.0),nullptr);
				world.insp_container[world.insp_precur].Lose(p.bag.tmpItem.cnt);
			}else if(p.bag.tmpItem.id == world.insp_container[world.insp_precur].id && !world.insp_container[world.insp_precur].IsFull())
			{	//若tmpItem为同类物品，则意为再放一个 
				p.bag.tmpItem.Lose(1);
				world.insp_container[world.insp_precur].Gain(1,nullptr);
			}else if(world.insp_container[world.insp_precur].id == 0 && p.bag.tmpItem.id != 0)
			{	//若格子为空，则意为放下一个 
				p.bag.tmpItem.Lose(1);
				world.insp_container[world.insp_precur].id = 	p.bag.tmpItem.id;
				world.insp_container[world.insp_precur].Gain(1,nullptr);
			}
			SND("obtain");
		}
		p.bag.lastSwap = clock();
	}
	if(!K(VK_LBUTTON) && !K(VK_RBUTTON))
	{	//release mouse buttons
		p.bag.lastSwap = clock() - SWAP_CD;
		p.craft.lastCraft = clock() - CSYS_CRAFT_CD;
		p.craft.lastFirstCraft = clock() - CSYS_FIRST_CRAFT_CD;
	}
	if(ms_msg_trg) {
		mouse_msg msg = ms_msg;
		if(msg.is_wheel()) {
			if(clock() - lastWheel >= WHEEL_CD) {
				if(p.bag.isOpen) {
					if(msg.wheel < 0 && p.craft.top_cur < short(p.craft.active_recipes.size()-csys_item_cnt_up-1))
//						p.craft.MoveTopCur(1);
//						ClampA<short>(++p.craft.top_cur,-CSYS_ITEM_CNT/2+1, p.craft.active_recipes.size()-CSYS_ITEM_CNT/2-1);
						p.craft.top_cur++;
					else if(msg.wheel > 0 && p.craft.top_cur > short(-csys_item_cnt_up))
//						p.craft.MoveTopCur(-1);
//						ClampA<short>(--p.craft.top_cur,-CSYS_ITEM_CNT/2+1, p.craft.active_recipes.size()-CSYS_ITEM_CNT/2-1);
						p.craft.top_cur--;
				} else {
					if (p.bag.cur > 0 && p.bag.cur < HOTBAR_ITEM_CNT - 1)
						p.OnItemSwitch(p.bag.cur, p.bag.cur + msg.wheel < 0 ? 1 : -1);
					if(msg.wheel < 0)
						ClampA<short>(++p.bag.cur,0,HOTBAR_ITEM_CNT-1);
					else if(msg.wheel > 0)
						ClampA<short>(--p.bag.cur,0,HOTBAR_ITEM_CNT-1);
				}
				lastWheel = clock();
			}
		}
	}
	RefreshPreblock();
	if(p.bag.precur >= 0 && p.bag.precur < ITEM_CNT && p.bag.items[p.bag.precur].id != 0)
	{
		cur_itemtag = p.bag.items[p.bag.precur].GetDescription();
	}else if(uid craft_precur != -1 && uid craft_precur + p.craft.top_cur < p.craft.active_recipes.size()
	&& p.craft.active_recipes.at(uid craft_precur + p.craft.top_cur) < recipes.size()) 
	{
		cur_itemtag = recipes.at(p.craft.active_recipes.at(uid craft_precur + p.craft.top_cur)).dst.GetDescription();
	}else if(p.inspblock != nullptr && world.insp_precur != -1)
	{
		cur_itemtag = world.insp_container.at(world.insp_precur).GetDescription();
	}
	else
		cur_itemtag.clear();
	XCPT_R
}
BLOCK_ID ItemToBlock(ITEM_ID id) {
	for(BLOCK_ID i = 0; i < blockdata.size(); ++i)
		if(itemdata.at(id).en_name == blockdata.at(i).en_name)
			return i;
	ErrorLogTip(NULL,"无法根据物品ID获取对应方块ID","ItemToBlock");
	return 0;
}

void UseItem(long x, long y, bool reach) {
	//使用物品
	XCPT_L
	ITEM_ID id = p.bag.HandItemCRef().id;
	ITEM_TYPE itype = p.bag.HandItemCRef().GetType();

	if(itype == ITT_NULL || itype == ITT_MATERIAL)
		return;	//没啥事就滚蛋
	if(HaveCDItemType(itype) && !p.TimeToUse())
		return;	//冷却中
	bool eaten = false;
	Block& bk = (itype == ITT_CLOSE_WEAPON ? 
				world.GetBlock(p.x, p.y, "KeyJ")
			:
				world.GetBlock(x, y, "KeyJ"));
	if (itype == ITT_CLOSE_WEAPON || itype == ITT_PICKAXE || itype == ITT_AXE)
	{	//近战武器 / 镐 / 斧
		ENTITY_ID eid = itemdata.at(p.bag.HandItemCRef().id).GetEntityID();
		DIR dir = GetDir4(x - p.x, y - p.y);
		long life = itemdata.at(p.bag.HandItemCRef().id).GetEntityLife();
		short att = itemdata.at(p.bag.HandItemCRef().id).GetAttackDamage();
		KBLEVEL kblvl = itemdata.at(p.bag.HandItemCRef().id).GetKnockbackLevel();
		string etag = "{Life:";
		etag += ToString(life);
		etag += "L,Owner:";
		etag += ToString(p.uuid);
		etag += "L,Attack:";
		etag += ToString(att);
		etag += "s,KnockbackLevel:";
		etag += ToString(kblvl);
		etag += "s,FromItemID:";
		etag += ToString(p.bag.HandItemCRef().id);
		etag += "s}";
		auto eptr = SpawnEntity(eid, p.x, p.y, dir, etag.c_str());
		if (itype == ITT_PICKAXE) {
			//镐是用来挖方块的 也可以砍树
			if (OuttaWorld(x, y) || !reach || bk.HaveClass(BCLASS_AIR))
			{
				SND("weak_attack"+ToString(RandomRange(1,4)));
				p.lastAttack = clock();
				return;
			}
			short power = itemdata.at(id).GetPower();
			short power_lvl = itemdata.at(id).GetPowerLevel();
			bool ret = world.GetBlock(x, y, "KeyJ").Damage(power, power_lvl, x, y);
			if (!ret) {
				//			ErrorLogTip(NULL,"镐子不够强，挖不动");
			}
			p.lastPutOrDigBlock_coord = LCOORD{ x,y };
			TrackLog("使用镐子");
			p.bag.HandItem().Damage(1);
		}
		else if (itype == ITT_AXE) {
			//斧子就是用来砍树类的 砍树效果比镐子好
			if (OuttaWorld(x, y) || !reach || !bk.HaveClass(BCLASS_TREE))
			{
				SND("weak_attack" + ToString(RandomRange(1, 4)));
				p.lastAttack = clock();
				return;
			}
			short power = itemdata.at(id).GetPower();
			short power_lvl = itemdata.at(id).GetPowerLevel();
			bool ret = world.GetBlock(x, y, "KeyJ").Damage(power, power_lvl, x, y);
			if (!ret) {
				//			ErrorLogTip(NULL,"斧子比较菜，砍不动");
			}
			p.lastPutOrDigBlock_coord = LCOORD{ x,y };
			TrackLog("使用斧子");
			p.bag.HandItem().Damage(1);
		}
		else {

		}
	}
	else if(itype == ITT_BLOCK) {
		//放置方块
		if(OuttaWorld(x,y) || bk.id != 0 && !bk.HaveClass(BCLASS_REPLACEABLE) || !reach 		//出界或已有方块
		|| ExistEntityHere(x,y) != nullptr
		|| (x==p.x&&y==p.y&&!blockdata.at(ItemToBlock(id)).HaveClass(BCLASS_PASSABLE)))	//或欲要在当前位置放不可通过的方块 
			return;
		world.SetBlock(x,y, ItemToBlock(id), biome_void, "KeyJ", p.bag.HandItemCRef().GetItemBlockTag().ToString().c_str());
		p.lastPutOrDigBlock_coord = LCOORD{ x,y };
		p.bag.HandItem().Lose(1);
		if (blockdata.at(ItemToBlock(id)).data.HaveKey("PlacedSound"))
			SND("block\\" + blockdata.at(ItemToBlock(id)).data.GetString("PlacedSound"));
		else if(blockdata.at(ItemToBlock(id)).data.HaveKey("PlacedSoundFamily"))
			PlayFamilySound(ParseSoundFamily(blockdata.at(ItemToBlock(id)).data.GetString("PlacedSoundFamily")), SNDFW_PLACE);
		else
			PlayFamilySound(blockdata.at(ItemToBlock(id)).snd_family, SNDFW_PLACE);
		TrackLog("放置方块");
	} else if(itype == ITT_BOW || itype == ITT_GUN || itype == ITT_RPG || itype == ITT_LAUNCHER
		)	//远程武器
	{
		bool has=false;
		bool needItem = true;
		if (itemdata.at(id).data.HaveKey("ProjNeedItem") 
			&& itemdata.at(id).data.GetShort("ProjNeedItem")==0)
		{
			needItem = false;
		}
		ITEM_ID proj;
		if (needItem)
		{
			proj = p.GetProjectile(itype, &has);
			if (!has)	return;	//no corresponding projectiles
		}
		else {
			proj = itemdata.at(id).data.GetShort("EntityID");
		}
		short magicCost = 0;
		if (itemdata.at(id).data.HaveKey("MagicCost"))
		{
			magicCost = itemdata.at(id).data.GetShort("MagicCost");
			if (p.mp < magicCost)
				return;
		}
		DIR dir = GetDir8(x-p.x,y-p.y);
		long _x=p.x,_y=p.y;
		DirOffsetPos(_x,_y,dir,"UseItem");
		if(world.GetBlock(_x,_y,"UseItem").IsBarrier())	return;	//against a block
		if(needItem)
			p.LoseItem(proj,1);
		if (magicCost > 0)
			p.LoseMagic(magicCost);
		ENTITY_ID proj_eid = (itype == ITT_BOW || itype == ITT_GUN || itype == ITT_RPG 
								?
								itemdata.at(proj).GetEntityID()
								:
								itemdata.at(id).GetEntityID());
		p.bag.HandItem().Damage(1);
		SpawnProjectile(proj_eid,
//		_x,_y, 
		p.x,p.y,
		dir, p.uuid, 
		     entitydata.at(proj_eid).GetExtraDamage()+itemdata.at(p.bag.HandItemCRef().id).GetAttackDamage(),
			 itemdata.at(id).GetProjectileMovingCD(),itemdata.at(p.bag.HandItemCRef().id).GetRange(),
			itemdata.at(id).GetKnockbackLevel());
	}
	else if (itype == ITT_THROWABLE)
	{	//一次性投掷物
		DIR dir = GetDir8(x - p.x, y - p.y);
		long _x = p.x, _y = p.y;
		DirOffsetPos(_x, _y, dir, "UseItem");
		if (world.GetBlock(_x, _y, "UseItem").IsBarrier())	return;	//against a block
		SpawnProjectile(itemdata.at(p.bag.HandItemCRef().id).GetEntityID(),
			//		_x,_y, 
			p.x, p.y,
			dir, p.uuid,
			itemdata.at(p.bag.HandItemCRef().id).GetAttackDamage(),
			itemdata.at(id).GetProjectileMovingCD(), itemdata.at(p.bag.HandItemCRef().id).GetRange(),
			itemdata.at(id).GetKnockbackLevel());
		
		p.bag.HandItem().Lose(1);
	}
	else if(itype == ITT_FOOD)
	{
		short food = itemdata.at(id).GetFood();
		eaten = p.Eat(food);
		if(eaten)
		{
			(p.bag.tmpItem.id == 0 ? p.bag.HandItem() : p.bag.tmpItem).Lose(1);
			TrackLog("吃东西") ;
		}
	}
	else{
		return;
	}
	ExtraAction(itemdata.at(id).data, (Creature*)&p);
_last:
	//item using success
	CheckAchvUses(p.bag.HandItemCRef());
	if (itemdata.at(id).data.HaveKey("UseSound"))
	{
		string name = itemdata.at(id).data.GetString("UseSound");
		SND("item\\" + name);
	}
	else {
		if (itype == ITT_CLOSE_WEAPON)
			SND("weak_attack" + ToString(RandomRange(1, 4)));
		else if (itype == ITT_FOOD && eaten)
			SND("eat");
		else if (itype == ITT_BOW)
			SND("item\\bow_shoot");
		else if (itype == ITT_GUN)
			SND("item\\blast");
		else if (itype == ITT_THROWABLE)
			SND("throw");
		else if (itype == ITT_CROSSBOW)
			SND("item\\crossbow_shoot"+ToString(RandomRange(1,3)));
		else if (itype == ITT_RPG || itype == ITT_LAUNCHER)
			SND("launch");
	}
	p.lastAttack = clock();
	XCPT_R
}
void KeyJ() {
	long _x=p.x,_y=p.y;
	DirOffsetPos(_x,_y, p.dir,"KeyJ");
	UseItem(_x,_y,true);
	if(!HaveCDItemType(p.bag.HandItemCRef().GetType()))
		while(K('J'));	//等待释放 
}
void CloseContainer()
{
	if (blockdata.at(p.inspblock->id).data.HaveKey("CloseSound"))
	{
		string sname = blockdata.at(p.inspblock->id).data.GetString("CloseSound");
		SND("block\\"+sname);
	}
	world.SaveInspectedContainer(*p.inspblock);
	world.insp_container.clear();	//释放内存 
	p.inspblock = nullptr;
}
void OperateBlock(long x, long y, bool reach) {
	//操纵方块
	XCPT_L
	if(!reach || clock() - p.lastOperate < OPERATE_CD)
		return;
	p.lastOperateBlock_coord = LCOORD{ x,y };
	Block& bk = world.GetBlock(x,y,"KeyK");
	if(bk.HaveClass(BCLASS_DOOR) && (x != p.x || y != p.y)) {
		if (!bk.tag.HaveKey("Open")) {
			//abnormal
			bk.tag.AddItem("Open", "0s");
		}
		string openraw = bk.tag.GetValueRaw("Open");
		if(openraw.size() >= 2)
			bk.tag.SetShort("Open", 1-Convert<char,short>(openraw[0]));
		if (Convert<char, short>(openraw[0]))
			SND("block\\door_open");
		else
			SND("block\\door_close");
		TrackLog("调整门的状态");
	}else if(bk.HaveClass(BCLASS_CONTAINER))
	{	//查看容器内的物品 
		if(p.inspblock!=nullptr && p.inspblock_coord.X == x && p.inspblock_coord.Y == y)
		{	//关闭 
			CloseContainer();
		}else{
			p.bag.isOpen = true;
			world.UpdateInspectedContainer(bk);
			p.inspblock = &bk;
			p.inspblock_coord.X = x;
			p.inspblock_coord.Y = y;
			if (blockdata.at(p.inspblock->id).data.HaveKey("OpenSound"))
			{
				string sname = blockdata.at(p.inspblock->id).data.GetString("OpenSound");
				SND("block\\"+sname);
			}
		}
	}else if(bk.HaveClass(BCLASS_PICKUP))
	{
		bk.Break(x,y);
	}else if(bk.HaveClass(BCLASS_BURNER) && p.bag.HandItemCRef().IsFuel())
	{	//添加燃料 
		short val = p.bag.HandItemCRef().GetFuelValue();
		bk.AddFuel(val);
		p.bag.HandItem().Lose(1);
	}
	else if(bk.HaveClass(BCLASS_FRUITFUL))
	{	//采摘附属物 
		Item fruits = bk.PickFruit();
		if(fruits.id != 0 && fruits.cnt > 0)
		{
			p.GainItem(fruits.id,fruits.cnt,fruits.tag.ToString().c_str());
		}
	}
	else if(bk.HaveClass(BCLASS_CRAFT))
	{
		p.bag.isOpen = true;
	}
	else{
		return;
	}
	p.lastOperate = clock();
	bk.AdjustShape();
	XCPT_R
}
void KeyK() {
	long x=p.x,y=p.y;
	DirOffsetPos(x,y,p.dir,"KeyK");
	if(OuttaWorld(x,y))
		return;
	OperateBlock(x,y,true);
}

void ClearGameMemories()
{	//清空进入游戏运行时的内存 
	world.Clear();
	entities.clear();
	players.clear();
	ITEM_FS = ITEM_DEF_FS;
}
void ChatBoxControl()
{
	if (!uid bPaused && K('T') && !p.chatOpen)
	{
		SND("click");
		p.chatOpen = true;
		p.chatbox.Init();
		p.chatbox.Enable();
		p.chatbox.Show();
		while (K('T'));
	}
	else if (!uid bPaused && K(VK_ESCAPE) && p.chatOpen)
	{
		SND("click");
		p.chatOpen = false;
		//p.chatbox.Hide();
		p.chatbox.Dispose();
		while (K(VK_ESCAPE));
	}
	if (p.chatOpen)
	{
		if (p.chatbox.Dead())
			p.chatbox.Init();
		if (!p.chatbox.IsFocused())
			p.chatbox.SetFocus();
		if (K(VK_RETURN))
		{
			string text = p.chatbox.GetText();
			AddChatItem("[{text:\"<"+p.name+"> \",color:#white},{text:\""+text+"\",color:#light_gray}]");
			p.chatbox.ClearText();
			p.chatOpen = false;
			//p.chatbox.Hide();
			//p.chatbox.Disable();
			p.chatbox.Dispose();
		}
	}
}

void GameControl() {
	XCPT_L
	if(!g_playing)	return;

	if(p.IsAlive() && !uid bPaused && !p.chatOpen) {
		if(K('A') || K(VK_LEFT)) {
			p.dir = LEFT;
			if((opmode.spectator || !IsBarrier(p.x,p.y,p.dir)) && !FacingEntity(p.x,p.y,p.dir, true)) {	//outtaworld inside
				p.TryToMove(LEFT);
			}
		} else if(K('W') || K(VK_UP)) {
			p.dir = UP;
			if ((opmode.spectator || !IsBarrier(p.x, p.y, p.dir)) && !FacingEntity(p.x, p.y, p.dir, true)) {	//outtaworld inside
				p.TryToMove(UP);
			}
		} else if(K('S') || K(VK_DOWN)) {
			p.dir = DOWN;
			if ((opmode.spectator || !IsBarrier(p.x, p.y, p.dir)) && !FacingEntity(p.x, p.y, p.dir, true)) {	//outtaworld inside
				p.TryToMove(DOWN);
			}
		} else if(K('D') || K(VK_RIGHT)) {
			p.dir = RIGHT;
			if ((opmode.spectator || !IsBarrier(p.x, p.y, p.dir)) && !FacingEntity(p.x, p.y, p.dir, true)) {	//outtaworld inside
				p.TryToMove(RIGHT);
			}
		}
		if(K('E')) {
			if(clock() - p.bag.lastOpen <= BAG_OPEN_CD)
				return;
			p.bag.isOpen = !p.bag.isOpen;
			if(!p.bag.isOpen) {
				if(p.inspblock != nullptr)
					CloseContainer();
				if(uid craft_precur != -1)
					uid craft_precur = -1;
				if(p.bag.tmpItem.id != 0) {
					//				MsgSndTip();
					if(p.bag.items[p.bag.tmpItemFrom].id != 0) {
						//多次取代后的结果
						//					DebugLog("multireplacing");
						short remain = p.GainItem(p.bag.tmpItem.id,p.bag.tmpItem.cnt,p.bag.tmpItem.tag.ToString().c_str());
						if(remain > 0) {	//directly get items
							//bag's full
							string etag;
							etag = "{Item:"+ToString(p.bag.tmpItem.id)+"s,Count:"+ToString(p.bag.tmpItem.cnt)+"s";
							if(!p.bag.tmpItem.tag.Empty())
							{
								etag += ",tag:";
								etag += p.bag.tmpItem.tag.ToString();
							}
							etag += "}";
							SND("obtain");
							SpawnItem(p.x, p.y, etag.c_str());
						}
						p.bag.tmpItem.Clear();
						p.bag.lastOpen = clock();
						return;
					}
					//				DebugLog("swap");
					p.bag.items[p.bag.tmpItemFrom].SwapWith(p.bag.tmpItem, p.bag.lastSwap,false);
				}
			}
			p.bag.lastOpen = clock();
		}
		if(p.bag.isOpen && p.IsAlive()) {
			if(clock() - lastWheel >= WHEEL_CD) {
				if(K(VK_OEM_COMMA)) {	//<,
					p.craft.MoveTopCur(-1);
				} else if(K(VK_OEM_PERIOD)) {	//>.
					p.craft.MoveTopCur(1);
				}
				lastWheel = clock();
			}
			if(p.craft.firstCrafted == 1 && clock() - p.craft.lastFirstCraft > CSYS_FIRST_CRAFT_CD_MAX
			        || p.craft.firstCrafted == 2 && clock() - p.craft.lastCraft > CSYS_FIRST_CRAFT_CD) {
				p.craft.firstCrafted = 0;
			}
			if(!p.craft.waitForRelease
			        && K(VK_RETURN)
			        && p.craft.Cur() < p.craft.active_recipes.size()
			        && ((p.craft.firstCrafted != 1 && clock() - p.craft.lastCraft >= CSYS_CRAFT_CD)
			            ||	(p.craft.firstCrafted == 1 && clock() - p.craft.lastFirstCraft >= CSYS_FIRST_CRAFT_CD))
				    && (p.bag.tmpItem.id == 0 || p.bag.tmpItem.id == recipes.at(p.craft.active_recipes.at(p.craft.Cur())).dst.id 
												&& !p.bag.tmpItem.IsFull())
				) {
				//Craft
				p.craft.CraftOnce(p.craft.active_recipes.at(p.craft.Cur()), &p.bag);
				if(p.craft.firstCrafted == 0) {
					p.craft.firstCrafted = 1;
					p.craft.lastFirstCraft = clock();
				} else if(p.craft.firstCrafted == 1) {
					p.craft.firstCrafted = 2;
					p.craft.lastCraft = clock();
				} else {
					p.craft.lastCraft = clock();
				}
				SND("obtain");
				TrackLog("合成一次");
			}
		}
		if(p.IsAlive())
			for(char hi = '0'; hi < '0'+HOTBAR_ITEM_CNT; ++hi)
				if (K(hi))
				{
					auto neo = (Convert<char, short>(hi) + 9) % 10;
					if (p.bag.cur != neo)
						p.OnItemSwitch(p.bag.cur, neo);
					p.bag.cur = neo;
				}
					
		if(K('J') && p.IsAlive()) {
			KeyJ();
		} else if(K('K') && p.IsAlive()) {
			KeyK();
		} else if(K(VK_BACK) && clock() - p.lastDiscard >= DISCARD_CD)
		{
			long _x=p.x, _y=p.y;
			DirOffsetPos(_x,_y,p.dir,"GameControl");
			if(OuttaWorld(_x,_y) || world.GetBlock(_x,_y,"GameControl").IsBarrier())
				return;	//不能丢物品 
			if(p.bag.isOpen && p.bag.precur != -1)
			{
				short _cnt = (K(VK_SHIFT) ? p.bag.items[p.bag.precur].cnt : 1);
				SpawnItem(_x,_y,p.bag.items[p.bag.precur].id, _cnt,
						p.bag.items[p.bag.precur].tag.ToString().c_str());
				p.bag.items[p.bag.precur].Lose(_cnt);
			}else if(!p.bag.isOpen && p.bag.HandItemCRef().id != 0){
				short _cnt = (K(VK_SHIFT) ? p.bag.HandItemCRef().cnt : 1);
				SpawnItem(_x,_y,p.bag.HandItemCRef().id, _cnt,
						p.bag.HandItemCRef().tag.ToString().c_str());
				p.bag.HandItem().Lose(_cnt);
			}
			SND("obtain");
			TrackLog("丢弃物品");
			p.lastDiscard = clock();
		} else if(K(VK_DELETE) && clock() - p.lastDiscard >= DISCARD_CD)
		{
			if(p.bag.isOpen && p.bag.precur != -1)
			{
				short _cnt = (K(VK_SHIFT) ? p.bag.items[p.bag.precur].cnt : 1);
				p.bag.items[p.bag.precur].Lose(_cnt);
			}else if(!p.bag.isOpen && p.bag.HandItemCRef().id != 0){
				short _cnt = (K(VK_SHIFT) ? p.bag.HandItemCRef().cnt : 1);
				p.bag.HandItem().Lose(_cnt);
			}
			SND("obtain");
			TrackLog("删除物品");
			p.lastDiscard = clock();
		}
		/*if(K(VK_ESCAPE)) {
			//Escape
			Exit();
			return;
		} 
		else */
		if(K(VK_CONTROL)) {
			while(K(VK_CONTROL));
			SND("click");
			options.blocktag_switch = !options.blocktag_switch;
		}else if(K(VK_F3))
		{
			while(K(VK_F3));
			options.show_debug_info = !options.show_debug_info;
		}
#ifndef NOCHEATKEYS
		if(K(VK_F8) && p.IsAlive()) {
			SND("click");
			opmode.spectator = !opmode.spectator;
			TrackLog("调整旁观者状态");
		}else if(K(VK_F7) && p.IsAlive())
		{
			p.hp = 0;
			p.lastDeath = clock();
			TrackLog("Death") ;
		}else if(K(VK_F5))
		{
			world.time = World::Sunrise;
		}else if(K(VK_F6))
		{
			world.time = World::Sunset;
		}else if(K(VK_F9))
		{
			while(K(VK_F9));
//			SpawnEntity(Choice({7,8,9,10,11,12,13,15,16}),p.x+Choice({-1,1}),p.y+Choice({-1,1}),LEFT);
			SpawnEntity(Choice({17,18,19}),p.x+Choice({-1,1}),p.y+Choice({-1,1}),LEFT);
		}
#endif
	}
	ChatBoxControl();
	if(K(VK_ESCAPE) && !p.chatOpen
	&& (uid bPaused || p.IsAlive())	//死亡时不能开菜单 
	)
	{
		if (p.inspblock != nullptr)
			CloseContainer();
		else {
			uid bPaused = !uid bPaused;
		}
		while (K(VK_ESCAPE));
	}
	//below are mouse message approaches
	if(mousemsg()) {
		ms_msg = getmouse();
		GetMousePos(ms_msg.x,ms_msg.y);
		ms_msg_trg = true;
	} else
		ms_msg_trg = false;
	if(!uid bPaused)
	{
		//Below are crafting mouse control
		if(!p.craft.waitForRelease && p.IsAlive()
		        && K(VK_RBUTTON)
		        && p.craft.Cur() < p.craft.active_recipes.size()
		        && uid craft_precur + p.craft.top_cur == p.craft.Cur()
		        && ((p.craft.firstCrafted != 1 && clock() - p.craft.lastCraft >= CSYS_CRAFT_CD)
		            ||	(p.craft.firstCrafted == 1 && clock() - p.craft.lastFirstCraft >= CSYS_FIRST_CRAFT_CD))
			    && (p.bag.tmpItem.id == 0 || p.bag.tmpItem.id == recipes.at(p.craft.active_recipes.at(p.craft.Cur())).dst.id
					                             && !p.bag.tmpItem.IsFull())
			    ) {
			//Craft
			p.craft.CraftOnce(p.craft.active_recipes.at(p.craft.Cur()), &p.bag);
			if(p.craft.firstCrafted == 0) {
				p.craft.firstCrafted = 1;
				p.craft.lastFirstCraft = clock();
			} else if(p.craft.firstCrafted == 1) {
				p.craft.firstCrafted = 2;
				p.craft.lastCraft = clock();
			} else {
				p.craft.lastCraft = clock();
			}
			SND("obtain");
			p.craft.lastRecipe = p.craft.active_recipes.at(p.craft.Cur());
		} else if(K(VK_LBUTTON) && p.IsAlive()
		          && uid craft_precur > 0
		          && (clock() - p.craft.lastChooseNearRecipe >= CSYS_CHOOSE_NEAR_RECIPE_CD)) {
			p.craft.top_cur = p.craft.top_cur - csys_item_cnt_up + uid craft_precur;
			p.craft.LimitTopCur();
			p.craft.lastChooseNearRecipe = clock();
		}
		if(!p.craft.waitForRelease && p.IsAlive()
		        && p.craft.Cur() < p.craft.active_recipes.size()
		        && (p.craft.lastRecipe == 0 || p.craft.active_recipes.at(p.craft.Cur()) != p.craft.lastRecipe)
		        && (K(VK_RBUTTON) || K(VK_RETURN))) {
			//What is going to be crafted is different from the previous one
			p.craft.waitForRelease = true;	//To prevent miscrafting
			p.craft.firstCrafted = 0;
		} else if(p.craft.waitForRelease && !K(VK_RBUTTON) && !K(VK_RETURN)) {
			//When all the crafting keys are released
			p.craft.waitForRelease = false;	//Then cancel it
					p.craft.firstCrafted = 0;
		}
		if(p.IsAlive() && ms_msg.x < rui_left-(p.bag.isOpen?exten_w:0) && ms_msg.x > 0 && ms_msg.y > 0 && ms_msg.y < uiy
		&& (p.inspblock == nullptr || !(ms_msg.x >= world.insp_ctn_left && ms_msg.y >= world.insp_ctn_top && ms_msg.x <= insp_ctn_right && ms_msg.y <= insp_ctn_bottom))
		) {
			if(K(VK_LBUTTON)) {
				//左键
				float _dist = Distance(p.preblock_coord.X, p.preblock_coord.Y, p.x, p.y);
				UseItem(p.preblock_coord.X, p.preblock_coord.Y, p.CanReach(_dist));
				if(!HaveCDItemType(p.bag.HandItemCRef().GetType()))
					while(K(VK_LBUTTON));	//await for releasing
			} else if(K(VK_RBUTTON)) {
				//右键
				if(p.preblock != nullptr) {
					//At one block
					float _dist = Distance(p.preblock_coord.X, p.preblock_coord.Y, p.x, p.y);
					bool canReach = p.CanReach(_dist);
					OperateBlock(p.preblock_coord.X, p.preblock_coord.Y, canReach);
				}
			}
		}
	}

	XCPT_R
}
void DrawEntities() {
	XCPT_L
	if(entities.empty())	return;
	for(long i = 0; i < entities.size(); ++i)
		entities.at(i).Draw();
	XCPT_R
}
#define ITAG_FS (ITEM_FS*0.95)
#define ITAG_LN_RATIO 1.05f
#define ITAG_BOTTOM_GAP (30)
void DrawItemTag(vector<string>& itemtag, int x, int y) {
	
	XCPT_L
	if(!options.itemtag_switch || itemtag.empty())	return;
	size_t tag_w=1,tag_h=1;
	for(short i = 0; i < itemtag.size(); ++i)
		tag_w = max<size_t>(tag_w, itemtag.at(i).size()*ITAG_FS/2);
	tag_h = ITAG_FS* ITAG_LN_RATIO *itemtag.size();
	if(x+tag_w > uix)
		x-=(x+tag_w-uix);
	if(y+tag_h > uiy-ITAG_BOTTOM_GAP)
		y-=(y+tag_h-uiy+ITAG_BOTTOM_GAP);
	//以上是防出屏代码
	//setfillcolor(BLACK);
	//setlinestyle(PS_SOLID,0,2);
	//setcolor(YELLOW);
	//sidebar(x,y,x+tag_w+2,y+tag_h);
	setfont(ITAG_FS,0,"Determination Mono");
	for(size_t i = 0; i < itemtag.size(); ++i) {
		if (itemtag.at(i).at(0) == '*')
			setwcolor(YELLOW);
		else
			setwcolor(WHITE);
		pxprint_shadow(x,y+ITAG_FS * ITAG_LN_RATIO *i,itemtag.at(i));
	}
	XCPT_R
}
inline color_t BlockTagColor(color_t src) {
//	return WaveColor(InvertedColor(src));
	return WHITE;
}
#define BTAG_FS (g_block_px*0.85)			//字号 
#define BTAG_LIGHT_MIN 3	//亮度最低要求 
#define ETAG_FS (g_block_px*0.95)
void DrawMouseTip() {
	XCPT_L
	if(uid bPaused)	return;
	if(p.bag.tmpItem.id != 0) {
		p.bag.tmpItem.Draw(ms_msg.x, ms_msg.y, CM_TMPITEM, true);
	} else {
		//itemtag drawing
		if(!cur_itemtag.empty())
		    DrawItemTag(cur_itemtag, ms_msg.x, ms_msg.y);
		if(options.blocktag_switch && p.preblock != nullptr
		&& p.preblock->light >= BTAG_LIGHT_MIN
		&& (p.inspblock==nullptr
		     || !(ms_msg.x >= world.insp_ctn_left && ms_msg.x <= insp_ctn_right && ms_msg.y >= world.insp_ctn_top && ms_msg.y <= insp_ctn_bottom))
		) {
			//blocktag drawing
			BLOCK_ID bid = p.preblock->id;
			if(bid != 0) {
				color_t clr = blockdata.at(bid).txt_color;
				string name = blockdata.at(bid).cn_name;
				string en_name = blockdata.at(bid).en_name;
				//setfillcolor(BLACK);
				//setcolor(LIGHTCYAN);
				//setlinestyle(PS_SOLID,0,2);
				//sidebar(ms_msg.x+2,ms_msg.y+2,
				//        ms_msg.x+2+max(textwidth(name.c_str()),textwidth(en_name.c_str())),
				//        ms_msg.y+2+BTAG_FS*2);
				setfont(BTAG_FS,0,"Determination Mono");
				setwcolor(BlockTagColor(clr));
				pxprint_shadow(ms_msg.x+4,ms_msg.y+4,name);
				setwcolor(DARKGRAY);
				pxprint_shadow(ms_msg.x+4,ms_msg.y+4+BTAG_FS,en_name);
			}
		} else if(!options.blocktag_switch && p.preblock != nullptr && !uid block_op_tip.empty()) {
			//block operation tip drawing
			setfont(BTAG_FS,0,"Determination Mono");
			setwcolor(WHITE);
			pxprint_shadow(ms_msg.x+4,ms_msg.y+4,uid block_op_tip);
		}

		Entity* etptr;
		if (options.entitytag_switch 
			&& p.preblock != nullptr
			&& (etptr = ExistEntityHere(p.preblock_coord.X, p.preblock_coord.Y)) != nullptr)
		{
			if (etptr->GetType() == ETT_BLADE)
			{

			}
			else if (entitydata.at(etptr->id).data.HaveKey("MaxHp"))
			{
				setfont(ETAG_FS, 0, "Determination Mono");
				pxprint_format(ms_msg.x + 4, ms_msg.y - 4 - ETAG_FS,
					"[{text:\"" + entitydata.at(etptr->id).cn_name
					+ ":\",color:#white,waveColor:1s},{text:\"" + ToString(etptr->hp)
					+ "/"
					+ ToString(entitydata.at(etptr->id).GetMaxHp())
					+ "\",color:#"+string(etptr->GetType() == ETT_MONSTER ? "yellow" : "light_green")+",waveColor:1s}]",true);
			}
			else if(etptr->GetType() != ETT_ITEM){	//proj
				/*setfont(ETAG_FS*0.35, 0, "Determination Mono");
				pxprint_format(ms_msg.x + 4, ms_msg.y - 4 - ETAG_FS,
					"[{text:\"" + entitydata.at(etptr->id).cn_name
					+ "\",color:#light_gray,waveColor:1s}]", true); */
			}
			else {	//item
				setfont(ETAG_FS*0.5, 0, "Determination Mono");
				ITEM_ID iid; short cnt = 1;
				etptr->GetItemData(iid, cnt, nullptr);
				pxprint_format(ms_msg.x + 4, ms_msg.y - 4 - ETAG_FS,
					"[{text:\"" + itemdata.at(iid).cn_name + (cnt==1?"":"("+ToString(cnt)+")")
					+ "\",color:#light_gray,waveColor:1s}]", true);
			}
			
		}
	}//above are when the tmpItem is empty
	XCPT_R
}
void DrawCraftUI() {
	if(!p.bag.isOpen)
		return;
	p.craft.Draw();
}
void DrawNearbyMap(void)
{
	if(!world.nearbyMap)	return;
	putimage(uix/2-getwidth(world.nearbyMap)/2,uiy*0.2,world.nearbyMap);
}

void DrawDeathUI(void)
{
	static int title_fs = 80 * (uiy / 1080.0);
	XCPT_L
	if(!p.dead)	return;
	setfont(title_fs,0,"Determination Mono");
	setwcolor(LIGHTRED);
	midpxprint("重生中...", uiy * 0.47 ,0,rui_left);
	setfont(60*(uiy/1080.0),0, "Determination Mono");
	setwcolor(LIGHTRED);
	midpxprint(ToString(RESPAWN_CD/1000 - int((clock() - p.lastDeath) / 1000)),uiy*0.47+114 * (uiy / 1080.0),0,rui_left);
	XCPT_R
}
void DrawAchieved()
{
	static const int achv_fs = 54 * (uiy / 1080.0);
	//constexpr int desc_fs = 40;
	static ACHV_ID _lastAchieved = 0;
	int rt_w = uix * 0.3;
	if(p.prevAchv != 0 && (clock() - p.lastAchieve <= ACHV_SHOWN_TIME))
	{
		string s, desc;
		desc = achievement_data.at(p.prevAchv).description;
		s += "『"+ achievement_data.at(p.prevAchv).cn_name + "』";
		setwcolor(WHITE);
		setlinestyle(PS_SOLID,0,2);
		setfillcolor(BLACK);
		sidebar((rui_left-(p.bag.isOpen?exten_w:0))/2-rt_w/2, uiy*0.72-achv_fs-10,
				  (rui_left-(p.bag.isOpen?exten_w:0))/2+rt_w/2, uiy*0.72+achv_fs*2.0);
		setwcolor(YELLOW);
		setfont(achv_fs,0,"Determination Mono");
		midpxprint("获得成就!",uiy*0.72-achv_fs,0,rui_left-(p.bag.isOpen?exten_w:0));
		setwcolor(CYAN);
		midpxprint(s,uiy*0.72,0,rui_left-(p.bag.isOpen?exten_w:0));
		setwcolor(achievement_data.at(p.prevAchv).color);
		/*bar((rui_left-(p.bag.isOpen?exten_w:0))/2-achv_fs/2,uiy*0.72+achv_fs,
		    (rui_left-(p.bag.isOpen?exten_w:0))/2+achv_fs/2,uiy*0.72+achv_fs*2);*/
		midpxprint(achievement_data.at(p.prevAchv).shape,uiy*0.72+achv_fs,0,rui_left-(p.bag.isOpen?exten_w:0));
		//setfont(desc_fs,0,"Determination Mono");
		//setcolor(LIGHTGRAY);
		//midpxprint(desc,uiy*0.72+achv_fs*2,0,rui_left-(p.bag.isOpen?exten_w:0));
		if (_lastAchieved != p.prevAchv)
		{
			AddChatItem("[{text:\"" + p.name + "获得了成就 \",color:#white},{text:\"『" + achievement_data.at(p.prevAchv).cn_name + "』\",color:#light_blue},{text:\" (" + desc + ")\",color:#dark_gray}]");
			_lastAchieved = p.prevAchv;
		}
	}
}
#define DPM_BTN_PRE_COLOR PINK
#define DPM_BTN_COLOR LIGHTCYAN
#define DPM_BTN_COLOR_ADAPT(n) if(uid ui_pm_pre_id == n)\
								   setcolor(DPM_BTN_PRE_COLOR);\
							   else\
		                           setcolor(DPM_BTN_COLOR);
void DrawPausedMenu()
{
	XCPT_L
	if(!uid bPaused)	return;
	static int title_fs = 55 * (uiy/1080.0);
	static int btn_fs = Clamp<int>(45 * (uiy / 1080.0),30,65);
	static int ver_fs = 40 * (uiy / 1080.0);
	static int gap = btn_fs * 1.2;	
	static int _top = uiy * 0.2;
	constexpr int paused_intensity = 0xFF * 0.45f;
	imagefilter_blurring(NULL,0x01,paused_intensity);
	setfont(title_fs,0,"Determination Mono");
	setwcolor(YELLOW);
	midpxprint("游戏已暂停",_top);
	setfont(ver_fs,0,"Determination Mono");
	setwcolor(DARKGRAY);
	midpxprint(CURRENT_VERSION,uiy-ver_fs-30);
	setfont(btn_fs,0,"Determination Mono");
	DPM_BTN_COLOR_ADAPT(0)
	uid rt_pm_back = midpxprint("[Esc:回到游戏]",_top+title_fs+120);
	DPM_BTN_COLOR_ADAPT(1)
	uid rt_pm_achv = midpxprint("[A:成  就]",_top+title_fs+120+btn_fs+gap);
	DPM_BTN_COLOR_ADAPT(2)
	uid rt_pm_save = midpxprint("[S:保  存]",_top+title_fs+120+btn_fs*2+gap*2);
	DPM_BTN_COLOR_ADAPT(3)
	uid rt_pm_opt = midpxprint("[O:选   项]",_top+title_fs+120+btn_fs*3+gap*3);
	DPM_BTN_COLOR_ADAPT(4)
	uid rt_pm_sq = midpxprint("[Space:保存并退出]",_top+title_fs+120+btn_fs*4+gap*4,0,uix,1);
	XCPT_R
}
void DrawInspection()
{
	XCPT_L
	if(p.inspblock == nullptr)	return;
	static int insp_title_fs = 40*(uiy/1080.0);
	int _x=uix/2,_y=uiy/2;
	short _w=1,_h=1;
	color_t backclr = BLACK;
	backclr = blockdata.at(p.inspblock->id).GetContainerBgColor();
	_w = blockdata.at(p.inspblock->id).GetContainerWidth();
	_h = blockdata.at(p.inspblock->id).GetContainerHeight();
	_x = (scene_right - scene_left)/2 + (p.inspblock_coord.X - p.x)*g_block_px - _w*g_block_px/2;
	_y = GetPlayerDrawTop() + (p.inspblock_coord.Y - p.y) * g_block_px + insp_title_fs;
	//以下两句是防止UI出界 
	if(_y+_h*CONTAINER_ITEM_FS >= uiy-20) _y = uiy-20-_h*CONTAINER_ITEM_FS;
	if(_x+_w*CONTAINER_SLOT_W >= rui_left-(p.bag.isOpen?exten_w:0)-5)	_x = rui_left-(p.bag.isOpen?exten_w:0)-5-_w*CONTAINER_SLOT_W;
	int _ctnr_top = _y + insp_title_fs + 5;
	
	world.insp_w = _w;
	world.insp_h = _h;
	world.insp_ctn_left = _x;
	world.insp_ctn_top = _ctnr_top;
	
	setfillcolor(EGERGB(20,20,20));
	bar(_x,_y,_x + insp_title_fs * blockdata.at(p.inspblock->id).cn_name.size() / 2, _y + insp_title_fs);
	setfont(insp_title_fs,0,"Determination Mono");
	setwcolor(WHITE);
	pxprint(_x,_y,blockdata.at(p.inspblock->id).cn_name);
	for(short y = 0; y < _h; ++y)
		for(short x = 0; x < _w; ++x)
		{
			setfillcolor(backclr);
			bar(_x+x*CONTAINER_SLOT_W,_ctnr_top+y*CONTAINER_ITEM_FS,_x+(x+1)*CONTAINER_SLOT_W-2,_ctnr_top+(y+1)*CONTAINER_ITEM_FS);
			p.inspblock->GetContainerItem(x*_h+y).Draw(_x+5+x*CONTAINER_SLOT_W,_ctnr_top+y*CONTAINER_ITEM_FS,(world.insp_precur==x*_h+y?CM_PRECURRENT:CM_NORMAL),true, backclr, true);
		}
	for(short x = 1; x < _w; ++x)
	{
		setlinestyle(PS_SOLID,0,2);
		setcolor(InvertedColor(backclr));
		line(_x+x*CONTAINER_SLOT_W,_ctnr_top,_x+x*CONTAINER_SLOT_W,_ctnr_top+_h*CONTAINER_ITEM_FS);
	}
	XCPT_R
}
void DrawWeatherParticles()
{
	if (!options.show_weather)	return;
	XCPT_L
	for (size_t i = 0; i < world.wps.size(); ++i)
		world.wps.at(i).Draw();
	XCPT_R
}
void DrawChatItems()
{
	if (g_chat.empty())	return;
	size_t i = 1;
	for (auto riter = g_chat.rbegin(); i < p.chatOpen?CHATITEM_FULL_SHOW_CNT:CHATITEM_SHOW_CNT && riter != g_chat.rend(); riter++)
	{
		if (riter->IsActive() || p.chatOpen)
		{
			setfont(CHAT_FS, 0, "Determination Mono");
			pxsetalpha(0xFF * riter->GetShowRatio(p.chatOpen));
			pxprint_format(CHAT_LEFT, CHAT_BOTTOM - CHAT_FS * i, riter->format, true);
			pxsetalpha();
			++i;
		}
	}
}
void DrawFXDisplay()
{
	if (p.effects.empty())	return;
	int fx_left = 100*(uix/1920.0)+(p.bag.isOpen ? CSYS_LEFTBAR_W : 0);
	constexpr int fx_top = 20;
	int fx_right = (p.bag.isOpen ? rui_left - exten_w : rui_left);
	size_t i = 0;
	size_t rowCnt = (fx_right - fx_left - 1) / (EFF_FS + EFF_X_GAP);
	for (const auto& eff : p.effects)
	{
		int x = fx_left + (i % rowCnt) * (EFF_FS + EFF_X_GAP);
		int y = fx_top + (i / rowCnt) * (EFF_FS + EFF_Y_GAP);
		eff.Draw(x, y);
		++i;
	}
}
void DrawScene() {
	XCPT_L
	constexpr int death_intensity = 0xFF * 0.5f;

	cls();

	DrawSceneBlocks();
	DrawPlayer();
	DrawEntities();
	DrawDamageIndicators();
	if (options.show_weather)
	{
		if (world.wpid == WPID_RAINDROP)
		{
			imagefilter_blurring(NULL, 0, world.GetRainyAlpha());
		}
		else if (world.wpid == WPID_SNOWFLAKE)
		{
			PIMAGE img_scene = newimage();
			getimage(img_scene, scene_left, scene_top, scene_right - scene_left, scene_bottom - scene_top);
			putimage_alphablend(img_scene, uid white_screen, 0, 0, world.GetSnowyAlpha(), 0, 0, getwidth(img_scene), getheight(img_scene));
			imagefilter_blurring(img_scene, world.GetSnowyBlur(), 245);
			putimage(scene_left, scene_top, img_scene);
			delimage(img_scene);
		}
	}
	DrawWeatherParticles();
	DrawInspection();
	p.aboveTip.Draw();
	DrawChatItems();
	if(p.dead)
		imagefilter_blurring(NULL,0xFF,death_intensity);
	else if (p.GetHungryStatus() == HS_VERY_HUNGRY)
	{
		double _ratio = 1.0 - p.sp / double(p.sp * PL_VERY_HUNGRY_RATIO);
		imagefilter_blurring(NULL, 0xFF, 0xFF * _ratio);
	}
	DrawFXDisplay();
	DrawRightUI();
	DrawCraftUI();
	DrawMouseTip();
	if(K(VK_TAB) && ! uid bPaused)
		DrawNearbyMap();
	DrawAchieved();
	if(!p.IsAlive())
		DrawDeathUI();
	
	if(!uid bPaused && options.show_debug_info)
		DrawDebugInfo();
	DrawPausedMenu();

	delay_fps(300);
	XCPT_R
}

void CheckEntitiesUpdate() {
	XCPT_L
	if(entities.empty())	return;
	vector<Entity>::iterator iter;
	for(long i = 0; i < entities.size(); ++i) {
		if(entities.at(i).TimeToBeCleared()) {
			iter = entities.begin();
			long j = 0;
			while (iter != entities.end() && j != i) {
				iter++;
				j++;
			}
			iter = entities.erase(iter);
			continue;
		}
		//以上代替 CheckEntitiesLives()
		entities.at(i).Update();
	}
	XCPT_R
}
void CheckDeathCount()
{
	static int lastN = 0;
	if (!p.dead || !options.sound_switch)	return;
	if ((clock() - p.lastDeath) / 1000 != lastN)
	{
		SND("pling");
		lastN = (clock() - p.lastDeath) / 1000;
	}
}
void MajorChecks() {
	//主要更新
	XCPT_L
	CheckEntitiesUpdate();
	world.UpdateSurroundingLight();		//lightness
	//world.UpdateWeather();
	world.UpdateSurroundingSounds();
	
	CheckDeathCount();
	XCPT_R
}
void CheckEntitiesSpawn()
{
	if (!options.mob_spawning)	return;
	XCPT_L
	world.UpdateEntitySpawners();
	for (size_t _ = 0; _ < world.espawners.size(); ++_)
	{
		world.espawners.at(_).DoSpawn();
	}
	//spawn其实还有产卵的意思
	for (size_t i = 0; i < entities.size(); ++i)
	{
		if (entitydata.at(entities.at(i).id).data.HaveKey("LayEggID")
			&& entitydata.at(entities.at(i).id).data.HaveKey("LayEggCD"))
		{
			ITEM_ID egg_id = entitydata.at(entities.at(i).id).data.GetShort("LayEggID");
			long egg_cd = entitydata.at(entities.at(i).id).data.GetLong("LayEggCD");
			if (!entities.at(i).tag.HaveKey("LastEgg"))
			{
				entities.at(i).tag.AddItem("LastEgg", ToString(clock()) + "L");
				continue;
			}
			else {
				if (clock() - entities.at(i).tag.GetLong("LastEgg") >= egg_cd)
				{
					SpawnItem(entities.at(i).x, entities.at(i).y, egg_id, 1, nullptr);
					entities.at(i).tag.SetLong("LastEgg", clock());
					string aname = "plop";
					if (entitydata.at(entities.at(i).id).data.HaveKey("LayEggAudioName"))
						aname = entitydata.at(entities.at(i).id).data.GetString("LayEggAudioName");
					string sndname = "entity\\" + entitydata.at(entities.at(i).id).data.GetString("AudioFolderName") + "\\"
						+ aname;
					if (ExistFile(AUDIO_DIR + sndname + ".mp3"))
						SND(sndname);
				}
			}
		}
	}
	XCPT_R
}
void CheckRespawn(void)
{
	if(!p.IsAlive() && clock() - p.lastDeath > RESPAWN_CD)
	{
		SND("levelup");
		p.Birth();	//dead = false
		world.manager.Update(p.x, p.y);
	}
}

void RefreshPMPrecur()
{
	XCPT_L
	int mx=0,my=0;
	GetMousePos(mx,my);
	if(InRect(mx,my,uid rt_pm_back))
	{
		uid ui_pm_pre_id = 0;
	}else if(InRect(mx,my,uid rt_pm_achv))
	{
		uid ui_pm_pre_id = 1;
	}else if(InRect(mx,my,uid rt_pm_save))
	{
		uid ui_pm_pre_id = 2;
	}else if(InRect(mx,my,uid rt_pm_opt))
	{
		uid ui_pm_pre_id = 3;
	}else if(InRect(mx,my,uid rt_pm_sq))
	{
		uid ui_pm_pre_id = 4;
	}else 
		uid ui_pm_pre_id = -1;
	XCPT_R
}
#define ACD_TOP (uiy*0.12f)
#define ACD_BELOW_TOP (ACD_TOP+100)
#define ACD_ACHV_H 150
#define ACD_PAGE_ACHV_CNT ((uiy - ACD_BELOW_TOP - 40) / ACD_ACHV_H)
void DrawAchievementsDisplay()
{
	XCPT_L
	constexpr int title_fs = 60;
	constexpr int achv_fs = 45;
	constexpr int icon_fs = 50;
	constexpr int text_fs = 40;
	constexpr int info_fs = 35;
	constexpr int num_fs = 30;
	constexpr int achv_h = ACD_ACHV_H;
	static int top = ACD_TOP;
	static int rt_w = uix * 0.36f;
	static int below_top = ACD_BELOW_TOP;
	static int page_achv_cnt = ACD_PAGE_ACHV_CNT;
	constexpr color_t title_clr = YELLOW;
	constexpr color_t achv_clr = LIGHTGREEN;
	constexpr color_t def_clr = EGERGB(234,234,234);
	constexpr color_t desc_clr = EGERGB(122,122,122);
	constexpr color_t info_clr = GREEN;
	constexpr color_t num_clr = WHITE;
	cls();
	setfont(title_fs,0,"Determination Mono");
	setcolor(title_clr);
	midpxprint("成  就",top);
	setfont(info_fs,0,"Determination Mono");
	setwcolor(info_clr);
	char buf[80];
	sprintf(buf,"%d/%d  %.1f%%",p.achievements.size(),achievement_data.size()-1,100*p.achievements.size()/float(achievement_data.size()-1));
	midpxprint(buf, uiy - info_fs*1.8f,0,uix,2);
	setcolor(DARKGRAY);
	setfont(35,0,"Determination Mono");
	pxprint(100,uiy-50,"Esc:返回菜单");
	pxprint(100,uiy-200,"WS/↑↓:滚动");
	for(short j = 0; j < page_achv_cnt; ++j)
	{
		if(uid acd_top_cur+j >= achievement_data.size())	break;
//		const AchievementData& ad = achievement_data.at(p.achievements.at(uid acd_top_cur+j));
		const AchievementData& ad = achievement_data.at(uid acd_top_cur+j);
		bool unlocked = p.HaveAchieved(uid acd_top_cur+j);
		if(unlocked)
			setwcolor(CYAN);
		else
			setcolor(RED);
		rectangle(uix/2 - rt_w/2, below_top + achv_h * j - 2, uix/2 + rt_w/2, below_top+achv_h*j+achv_fs+8);
		setfont(num_fs,0,"Determination Mono");
		setcolor(unlocked?num_clr:LIGHTGRAY);
		pxprint(uix/2 - rt_w/2 + 30,below_top + achv_h * j, "#"+ToString(uid acd_top_cur+j));
		setfont(icon_fs,0,"Determination Mono");
		setwcolor(unlocked?ad.color:EGERGB(12,12,12));
		pxprint(uix/2 - 90,below_top + achv_h * j, ad.shape);
		setfont(achv_fs,0,"Determination Mono");
		setwcolor(unlocked?achv_clr:DARKGRAY);
		pxprint(uix/2 - 5,below_top + achv_h * j+2,ad.cn_name);
		setfont(text_fs,0,"Determination Mono");
		setwcolor(unlocked?def_clr:RED);
		midpxprint("【"+ad.definition+"】",below_top + achv_h * j+9+achv_fs,0,uix,unlocked?1:0);
		setcolor(unlocked?desc_clr:EGERGB(5,5,5));
		midpxprint(ad.description,below_top + achv_h * j+9+achv_fs+text_fs,0,uix,unlocked?2:0);
	}
	delay_fps(60);
	
	XCPT_R
}
#define ACHV_DRAW_CD 60
void PausedMenuControl_Achievements(void)
{
	XCPT_L
	DebugLog("进入成就列表");
	uid acd_top_cur=1;
	clock_t lastDraw = clock() - ACHV_DRAW_CD;
	clock_t lastKey = clock();
	while(g_playing && is_run())
	{
		if(clock() - lastDraw >= ACHV_DRAW_CD)
		{
			DrawAchievementsDisplay();
			lastDraw = clock();
		}
		if(K(VK_DOWN) || K('S'))
		{
			if(clock() - lastKey < KEY_CD)	continue;
			uid acd_top_cur++;
			lastKey=clock();
			ClampA<short>(uid acd_top_cur,1,achievement_data.size()-ACD_PAGE_ACHV_CNT+1);
		}else if(K(VK_UP) || K('W'))
		{
			if(clock() - lastKey < KEY_CD)	continue;
			uid acd_top_cur--;
			lastKey=clock();
			ClampA<short>(uid acd_top_cur,1,achievement_data.size()-ACD_PAGE_ACHV_CNT+1);
		}else if(K(VK_NEXT))
		{
			while(K(VK_NEXT));
			uid acd_top_cur += ACD_PAGE_ACHV_CNT;
			ClampA<short>(uid acd_top_cur,1,achievement_data.size()-ACD_PAGE_ACHV_CNT+1);
		}else if(K(VK_PRIOR))
		{
			while(K(VK_PRIOR));
			uid acd_top_cur -= ACD_PAGE_ACHV_CNT;
			ClampA<short>(uid acd_top_cur,1,achievement_data.size()-ACD_PAGE_ACHV_CNT+1);
		}else if(K(VK_CONTROL) && K(VK_TAB))
		{
			while(K(VK_CONTROL) && K(VK_TAB));
			uid acd_top_cur += 1;
			if(uid acd_top_cur > achievement_data.size()-ACD_PAGE_ACHV_CNT+1)
				uid acd_top_cur = 1;
			ClampA<short>(uid acd_top_cur,1,achievement_data.size()-ACD_PAGE_ACHV_CNT+1);
		}
		if(K(VK_ESCAPE))
		{
			while(K(VK_ESCAPE));
			break;
		}
	}
	DebugLog("退出成就列表");
	XCPT_R
}
void PausedMenuControl_Save(void)
{
	SaveOptions();
	Save();
	SND("pling");
}
void PausedMenuControl_Options(void)
{
	ReadOptions();
	EnterOptions();
}
void Exit()
{
	g_sndmgr.StopPlayMusic();
	g_sndmgr.StopPlayAllSound();
	SND("click");
	setfont(55*(uiy/1080.0),0,"Determination Mono");
	setcolor(YELLOW);
	midpxprint("存档中请稍候",uiy - 150,0,uix,15);
	delay_fps(60);
	DebugLog("退出游戏，正在存档...") ;
	PausedMenuControl_Save();
	ClearGameMemories();
	flushkey();
	flushmouse();
	delay(500);
	DebugLog("存档完毕");
	g_playing = false;
}
void PausedMenuControl_Exit(void)
{
	Exit();
}
void PausedMenuControl()
{
	XCPT_L
	if(K(VK_LBUTTON) && uid ui_pm_pre_id > -1 && uid ui_pm_pre_id < 5)
	{
		while(K(VK_LBUTTON));	//to avoid misoperation
		if(uid ui_pm_pre_id == 0)
		{
			SND("click");
			uid bPaused = false;
		}else if(uid ui_pm_pre_id == 1)
		{
			SND("click");
			PausedMenuControl_Achievements();
		}else if(uid ui_pm_pre_id == 2)
		{
			SND("click");
			PausedMenuControl_Save();
		}else if(uid ui_pm_pre_id == 3)
		{
			SND("click");
			PausedMenuControl_Options();
		}else if(uid ui_pm_pre_id == 4)
		{
			PausedMenuControl_Exit();	//snd inside
		}
	}
	if(kbhit())
	{
		if(K('A'))
		{
			while(K('A'));
			SND("click");
			PausedMenuControl_Achievements();
		}else if(K('S'))
		{
			while(K('S'));
			SND("click");
			PausedMenuControl_Save();
		}else if(K('O'))
		{
			while(K('O'));
			SND("click");
			PausedMenuControl_Options();
		}else if(K(VK_SPACE))
		{
			while(K(VK_SPACE));
			SND("click");
			PausedMenuControl_Exit();
		}
	}
	XCPT_R
} 
void CheckInspection()
{
	XCPT_L
	if(p.inspblock == nullptr)	return;
	if(Distance(p.x,p.y,p.inspblock_coord.X,p.inspblock_coord.Y) > PL_DEF_REACH_RANGE)
	{
		if (blockdata.at(p.inspblock->id).data.HaveKey("CloseSound"))
		{
			string sname = blockdata.at(p.inspblock->id).data.GetString("CloseSound");
			SND("block\\" + sname);
		}
		p.inspblock = nullptr;
	}
	XCPT_R
}
void CheckCreatureFX()
{
	for (auto& pl : players)
		pl.UpdateFX(&pl);
	for (auto& et : entities)
		et.UpdateFX(&et);
}
#define MINOR_CHECK_CD 500
#define CHUNKLOAD_UPD_SPEC_CD 800
#define CHUNKLOAD_UPD_CD 3000
void MinorChecks() {
	//次要更新
	static clock_t lastChunkUpdate = clock();
	XCPT_L
	p.craft.LimitTopCur();
	p.UpdateCraftPlaces();
	p.UpdateCraftRecipes();
	if (clock() - lastChunkUpdate >= (opmode.spectator?CHUNKLOAD_UPD_SPEC_CD:CHUNKLOAD_UPD_CD)
		&& (p.lastChunkCoord.X != ToChunkCoord(p.x)
		|| p.lastChunkCoord.Y != ToChunkCoord(p.y))
		)
	{
		p.OnChunkChanged();
		lastChunkUpdate = clock();
	}
	world.UpdateTime();
	world.UpdateBurnerFuelValues(SURROUNDING_RANGE);
	world.UpdateEnvironmentFX();
	p.CheckIndexesChanges();
	p.aboveTip.Update();
	CheckEntitiesSpawn();
	if (clock() - p.lastAttack >= TOUCH_BLOCK_SHAKE_TIME && p.lastPutOrDigBlock_coord.X != -1 && p.lastPutOrDigBlock_coord.Y != -1)
	{
		p.lastPutOrDigBlock_coord = LCOORD{ -1,-1 };
	}
	if (clock() - p.lastOperate >= TOUCH_BLOCK_SHAKE_TIME && p.lastOperateBlock_coord.X != -1 && p.lastOperateBlock_coord.Y != -1)
	{
		p.lastOperateBlock_coord = LCOORD{ -1,-1 };
	}
	world.UpdateEnvironmentChanges();
	world.UpdateNearbyMap();
	CheckAchvGets();
	CheckRespawn();
	CheckInspection();
	UpdateChatItems();
	CheckDamageIndicatorsLife();
	CheckCreatureFX();
	XCPT_R
}
/*#define MOUSETIP_CD DRAW_CD
DWORD ThrMouseTips(LPVOID none) {
	XCPT_L
	DebugLog("[ThrMouseTips] 开启鼠标提示功能");
	while(is_run() && g_playing) {
		RefreshMouseTips();
//		api_sleep(MOUSETIP_CD);
	}
	DebugLog("[ThrMouseTips] 关闭鼠标提示功能");
	XCPT_R
	return 0;
}*/
#define RCHECK_CD 1
DWORD ThrRealtimeChecks(LPVOID none) {
	XCPT_L
	DebugLog("[ThrRealtimeChecks] 开启游戏更新线程");
	clock_t lastCheck=clock();
	while(is_run() && g_playing) 
	{
		XCPT_L
		world.UpdateWeather();
		api_sleep(RCHECK_CD);
		XCPT_R
	}
	DebugLog("[ThrRealtimeChecks] 关闭游戏更新线程");
	XCPT_R
	return 0;
}
void InGame() {
	//游戏主体函数
	XCPT_L
	
	g_playing = true;
	uid bPaused = false;
	uid ui_pm_pre_id = -1;
	clock_t lastDraw=clock();
	clock_t lastMinorCheck=clock();
	lastWheel = clock();

	scene_left = 50 * (uix/1920.0);
	scene_right = uix * 0.7;
	scene_top = 20 * (uiy / 1080.0);
	scene_bottom = uiy - scene_top;
	AdaptItemFontSize();
	AdaptCraftItemCount();
	world.manager.RefreshChunkloadDistance();
	world.manager.LoadChunk(ToChunkCoord(p.x), ToChunkCoord(p.y));
	ClearChatItems();
//	CreateThread((LPTHREAD_START_ROUTINE)ThrMouseTips);
	CreateThread((LPTHREAD_START_ROUTINE)ThrRealtimeChecks);

	DebugLog("进入游戏主体");
	while(g_playing && is_run()) {
		if(clock() - lastDraw >= DRAW_CD) {
			DrawScene();
			lastDraw = clock();
		}
		RefreshMouseTips();
		if(!uid bPaused)
		{
			MajorChecks();	//主要更新 
			if(clock() - lastMinorCheck >= MINOR_CHECK_CD) {
				MinorChecks();	//次要更新 
				lastMinorCheck = clock();
			}
		}else{	//暂停中，游戏主菜单控制 
			RefreshPMPrecur(); 
			PausedMenuControl();
		}
		GameControl();
	}
	DebugLog("退出游戏主体");
	g_playing=false;
	XCPT_R
}

namespace nvcfg {
	//注：如果不用指针会导致进入主函数前的初始化，导致RE
	sys_edit* seed_box=nullptr;
	sys_edit* pname_box=nullptr;

	int seed_box_w = 350;
	int& seed_box_h = uid ui_nvcfg_text_fs;
	const color_t seed_box_color = PINK;

	int pname_box_w = 500;
	int& pname_box_h = uid ui_nvcfg_text_fs;
	const color_t pname_box_color = LIGHTGREEN;

	string tip;

#define SEEDBOX_INPUT_MAXLEN 16
#define PNAMEBOX_INPUT_MAXLEN 32
#define SEED_MAX (ULONG_MAX-1)

	void InitBoxes() {
		TrackLog("输入框初始化...");

		seed_box_w *= (uix / 1920.0);
		pname_box_w *= (uix / 1920.0);
		const int title_top = uiy * 0.15;
		const int text_top = uiy * 0.4;

		if(!seed_box)
			seed_box = new sys_edit();
		if(!pname_box)
			pname_box = new sys_edit();

		unsigned long seed = random(SEED_MAX);
//		srand(time(NULL));
//		unsigned long seed = rand()%(SEED_MAX+1);
		seed_box->create(false);	//单行
		seed_box->move(uix/2 - (nvcfg::seed_box_w+100+uid ui_nvcfg_text_fs*3)/2 + 100+uid ui_nvcfg_text_fs*4,
		               text_top);
		seed_box->size(seed_box_w, seed_box_h+6*(uiy/1080.0));
		seed_box->setmaxlen(SEEDBOX_INPUT_MAXLEN);
		seed_box->setbgcolor(GRAY_11);
		seed_box->setcolor(seed_box_color);
		seed_box->setfont(uid ui_nvcfg_text_fs, 0, "Determination Mono");
		seed_box->settext(ToString(seed).c_str());	//set  seed
		seed_box->visable(false);

		pname_box->create(false);	//单行
		pname_box->move(uix/2 - (nvcfg::seed_box_w+100+uid ui_nvcfg_text_fs*5)/2 + 100+uid ui_nvcfg_text_fs*4,
		                text_top+uid ui_nvcfg_text_fs+30);
		pname_box->size(pname_box_w, pname_box_h + 6 * (uiy / 1080.0));
		pname_box->setmaxlen(PNAMEBOX_INPUT_MAXLEN);
		pname_box->setbgcolor(GRAY_11);
		pname_box->setcolor(pname_box_color);
		pname_box->setfont(uid ui_nvcfg_text_fs, 0, "Determination Mono");
		pname_box->visable(false);

		pname_box->setfocus();
	}
	void DeleteBoxes() {
		seed_box->visable(false);
		seed_box->destory();
		delete seed_box;
		seed_box = nullptr;

		pname_box->visable(false);
		pname_box->destory();
		delete pname_box;
		pname_box = nullptr;
	}
}
void DrawNewVoxelCfg(bool firstDraw) {
	static const int title_top = uiy * 0.15;
	static const int text_top = uiy * 0.4;

	cls();

	setfont(uid ui_nvcfg_title_fs,0,"Determination Mono");
	setcolor(YELLOW);
	midpxprint("维度初始配置",title_top,0,uix,1,firstDraw?100:0);
	setfont(uid ui_nvcfg_text_fs,0,"Determination Mono");
	setcolor(LIGHTGRAY);
	pxprint(uix/2 - (nvcfg::seed_box_w+100+uid ui_nvcfg_text_fs*5)/2,text_top,"维度随机种子:",2,firstDraw?80:0);
	pxprint(uix/2 - (nvcfg::pname_box_w+100+uid ui_nvcfg_text_fs*3)/2,text_top+uid ui_nvcfg_text_fs+30,"你的名字:",2,firstDraw?90:0);
	if(firstDraw || !nvcfg::seed_box->isvisable() || !nvcfg::pname_box->isvisable()) {
		nvcfg::seed_box->visable(true);
		nvcfg::pname_box->visable(true);
	}

	if(!nvcfg::tip.empty()) {
		setfont(uid ui_nvcfg_text_fs,0,"Determination Mono");
		setcolor(LIGHTRED);
		midpxprint(nvcfg::tip,uiy*0.55);
	}

	setfont(uid ui_nvcfg_btn_fs,0,"Determination Mono");
	if(uid ui_nvcfg_pre_id == 0)
		setcolor(PINK);
	else
		setcolor(LIGHTCYAN);
	uid rt_nvcfg_done = midpxprint("[Ctrl+S] 开始构建",uiy*0.77,0,uix,0,firstDraw?30:0);
	if(uid ui_nvcfg_pre_id == 1)
		setcolor(PINK);
	else
		setcolor(LIGHTCYAN);
	uid rt_nvcfg_cancel = midpxprint("[Ctrl+W] 取消",uiy*0.77+uid ui_nvcfg_btn_fs*2,0,uix,3,firstDraw?30:0);

	delay_fps(60);
}
bool CheckNewVoxelCfg(unsigned long& seed, string& pname) {
	if(nvcfg::seed_box == nullptr) {
		nvcfg::tip = "错误：无效的种子编辑框指针";
		return false;
	}
	char szSeed[SEEDBOX_INPUT_MAXLEN] {0};
	nvcfg::seed_box->gettext(SEEDBOX_INPUT_MAXLEN,szSeed);
	stringstream ss;
	ss<<szSeed;
	if(ss.str().empty()) {
		nvcfg::tip = "种子不得为空";
		return false;
	} else if(ss.str().at(0) == '-') {
		nvcfg::tip = "种子不得为负";
		return false;
	}
	ss>>seed;

	char szPName[PNAMEBOX_INPUT_MAXLEN] {0};
	nvcfg::pname_box->gettext(PNAMEBOX_INPUT_MAXLEN,szPName);
	if(ToString(szPName).empty()) {
		nvcfg::tip = "你的名字不得为空";
		return false;
	}
	pname = ToString(szPName);
	return true;
}
#define NVCFG_DRAW_CD 60
bool EnterNewVoxelCfg(unsigned long& seed, string& pname) {
	DebugLog("进入新维度初始配置界面");
	clock_t lastDraw=clock();
	nvcfg::InitBoxes();
	bool firstDraw=true;

	while(is_run()) {
		if(clock() - lastDraw >= NVCFG_DRAW_CD) {
			DrawNewVoxelCfg(firstDraw);
			if(firstDraw)	firstDraw=false;
			lastDraw = clock();
		}
		if(mousemsg()) {
			mouse_msg msg = getmouse();
			if(InRect(msg.x,msg.y,uid rt_nvcfg_done)) {
				if(msg.is_left() && msg.is_up()) {
					SND("click");
					bool ret = CheckNewVoxelCfg(seed,pname);
					if(!ret)
						continue;
					nvcfg::DeleteBoxes();
					return true;
				} else
					uid ui_nvcfg_pre_id = 0;
			} else if(InRect(msg.x,msg.y,uid rt_nvcfg_cancel)) {
				if(msg.is_left() && msg.is_up()) {
					SND("click");
					nvcfg::DeleteBoxes();
					return false;
				} else
					uid ui_nvcfg_pre_id = 1;
			} else
				uid ui_nvcfg_pre_id = -1;
		}
		if(kbhit()) {
			if(K(VK_CONTROL)) {
				if(K('S')) {
					SND("click");
					bool ret = CheckNewVoxelCfg(seed,pname);
					if(!ret)
						continue;
					nvcfg::DeleteBoxes();
					return true;
				} else if(K('W')) {
					SND("click");
					nvcfg::DeleteBoxes();
					return false;
				}
			}
		}
	}
	nvcfg::DeleteBoxes();
}


long GetPlayerX(void) {
	return p.x;
}
long GetPlayerY(void) {
	return p.y;
}
short GetPlayerHp(void) {
	return p.hp;
}
short GetPlayerMp(void) {
	return p.mp;
}
short GetPlayerSp(void) {
	return p.sp;
}
short GetPlayerMaxHp(void) {
	return p.max_hp;
}
short GetPlayerMaxMp(void) {
	return p.max_mp;
}
short GetPlayerMaxSp(void) {
	return p.max_sp;
}
string GetPlayerName(void) {
	return p.name;
}
long GetPlayerShapeID() {
	return p.shapeID;
}
color_t GetPlayerColor() {
	return p.color;
}

template <typename _T>
void optprint(fstream& fout, const char* keyName, _T data) {
	fout<<keyName<<"=";
	fout<<data;
	fout<<endl;
}
template <typename _T>
bool optinput(fstream& fin, _T& data) {
	string line;
	fin>>line;
	//e.g.
	//01234567890 size=10  i=6 len=size-i
	//music=true
	for(short i = 0; i < line.size(); ++i) {
		if(line.at(i) == '=') {
			stringstream ss;
			ss<<line.substr(i+1,line.size()-i);
//		DebugLog("substr="+ss.str());
			ss>>data;
			return true;
		}
	}
	DebugLog("没有在本行配置数据中找到等号："+line,DLT_ERROR);
	return false;
}

bool SavePlayerData() {
	//第一个玩家的数据
	if(players.empty()) {
		ErrorLogTip(NULL,"没有任何玩家！","SavePlayerData");
		return false;
	}
	DebugLog("【保存】保存玩家数据:"+PLAYERDATA_PATH);
	fstream fout(PLAYERDATA_PATH,ios::out);

	fout<<"[PlayerBasicData]"<<endl;
	optprint(fout, "Version" , CURRENT_VERSION);
	optprint(fout, "PlayerName", p.name);
	optprint(fout,"UUID",p.uuid);
	optprint(fout, "X", p.x);
	optprint(fout, "Y", p.y);
	optprint(fout, "Hp", p.hp);
	optprint(fout, "MaxHp", p.max_hp);
	optprint(fout, "Mp", p.mp);
	optprint(fout, "MaxMp", p.max_mp);
	optprint(fout, "Sp", p.sp);
	optprint(fout, "MaxSp", p.max_sp);
	optprint(fout, "Luck", p.luck);
	string sachv = ToString(p.achievements);
	optprint(fout, "Achievements", sachv);
	fout<<"[Inventory]"<<endl;
	for(short i = 0; i < ITEM_CNT; ++i) {
		string itemStr = ToString(p.bag.items[i].id)+"^"+ToString(p.bag.items[i].cnt)+"^"+p.bag.items[i].tag.ToString();
		optprint(fout, ("Item"+ToString(i)).c_str(), itemStr);
	}

	fout.close();
	return true;
}
void CreateNewPlayer(string& pname) {
	XCPT_L

	if(!players.empty())	players.clear();

	Player p1;
	p1.FirstInit(pname);
	players.push_back(p1);

	SavePlayerData();

	XCPT_R
}

short g_nv_status=-1;

#define SAVEWORLD_NEWLINE_BLOCK_CNT 16
void SaveWorldBasic() {
	DebugLog("【保存】保存世界基础数据:"+WORLDDATA_PATH);
	fstream fout(WORLDDATA_PATH,ios::out);

	fout<<"[WorldBasicData]"<<endl;
	optprint(fout, "WorldWidth", world.manager.map_w);
	optprint(fout, "WorldHeight", world.manager.map_h);
	optprint(fout, "Time", world.time);

	fout.close();
}
void SaveWorldChunks()
{
	world.manager.SaveChunks();
}
void SaveWorld()
{
	SaveWorldBasic();
	SaveWorldChunks();
}
void ReadWorldBasic() {
	if(!ExistFile(WORLDDATA_PATH)) {
		ErrorLogTip(NULL,"世界基础数据丢失!","ReadWorldBasic");
		return;
	}
	world.Clear();

	DebugLog("【读取】读取世界基础数据:"+WORLDDATA_PATH);
	fstream fin(WORLDDATA_PATH,ios::in);
	string tmp;
	fin>>tmp;	//Header
	optinput(fin,world.manager.map_w);
	optinput(fin,world.manager.map_h);
	optinput(fin,world.time);
	if (world.time >= World::Sunset)
		world.sunrisen = false;
	else
		world.sunrisen = true;
	fin.close();
}
void ReadWorld()
{	//Please ensure that player coord has been read
	ReadWorldBasic();
	DebugLog("【读取】开始读取世界区块数据...");
	world.manager.Update(p.x, p.y);
	DebugLog("【读取】区块读取加载完成！加载数量="+ToString(world.manager.chunks.size()));
}

#define BIOME_DEF_CN_NAME "未知群系"
#define BIOME_DEF_EN_NAME "UnknownBiome"
class BiomeData {	//生物群系数据
	public:
		BIOME_ID id;
		string cn_name;
		string en_name;

		vector<Weight<BLOCK_ID>> blocks;
		vector<BiomeEntitySpawnData> espawn_data;
	private:
		vector<BLOCK_ID> _wblocks;

	public:
		BiomeData() {
			Init();
		}
		void Init() {
			id = biome_void;
			cn_name = BIOME_DEF_CN_NAME;
			en_name = BIOME_DEF_EN_NAME;
		}
		void SpawnWBlocks(void) {
			_wblocks.clear();
			for(short i = 0; i < blocks.size(); ++i)
				for(short j = 0; j < blocks.at(i).weight; ++j)
					_wblocks.push_back(blocks.at(i).value);
		}
		BLOCK_ID RandomBlock(void) {
			if(_wblocks.empty())
				SpawnWBlocks();
			return Choice(_wblocks);
		}
};
vector<BiomeData> biomedata;

BIOME_ID CnNameToBiome(const string& cn_name) {
	for(short i = 0; i < biomedata.size(); ++i)
		if(biomedata.at(i).cn_name == cn_name)
			return BIOME_ID(i);
	return biome_void;
}
void ReadBiomeData() {
	if(!ExistFile(BIOMEDATA_PATH)) {
		ErrorLogTip(NULL,"生物群系数据丢失","ReadBiomeData");
		return;
	}
	DebugLog("【读取】读取生物群系数据："+BIOMEDATA_PATH);
	vector<string> lines = ReadFileLines(BIOMEDATA_PATH);
	long i=1;
	while(i < lines.size()) {
		string line = lines.at(i);
		if(line.empty()) {
			++i;
			continue;
		}
		line = CSVResplitLine(line);

		stringstream ss;
		BiomeData bdt;
		ss<<line;

		USHORT uShort;
		ss>>uShort;
		bdt.id = BIOME_ID(uShort);
		ss>>bdt.cn_name;
		ss>>bdt.en_name;

		string tmp;
		ss>>tmp;
//		DebugLog("blocks str="+tmp);
		bdt.blocks = ParseWBlocks(tmp);

		string tmptmp = "";
		short c = 0;
		while (!ss.eof()) {
			ss >> tmp;
			if (c != 0)	//2,3,4etc
				tmptmp += ",";
			tmptmp += tmp;
			c++;
		}
		if(tmptmp.find('\"') != string::npos)
			tmptmp = strxhead(strxtail(tmptmp));
		auto _list = ParseList(tmptmp, "ReadBiomeData");
		for (int i = 0; i < _list.size(); ++i)
		{
			BiomeEntitySpawnData ed;
			ed.Parse(_list.at(i));
			bdt.espawn_data.push_back(ed);
		}

		biomedata.push_back(bdt);
		++i;
	}
}

BLOCK_ID GetBiomeRandomBlock(BIOME_ID biome) {
	BLOCK_ID bid=0;
	bid = biomedata.at(USHORT(biome)).RandomBlock();
	return bid;
}
void DrawDebugInfo() {
	static int dbg_text_fs = 35 * (uiy/1080.0);
	setfont(dbg_text_fs, 0, "Determination Mono");
	setcolor(LIGHTGRAY);
	//	pxprint(10,10,"X="+ToString(GetPlayerX())+" Y="+ToString(GetPlayerY()));
	//	pxprint(10,10,"L:"+ToString(-CSYS_ITEM_CNT/2+1)+" R:"+ToString(p.craft.active_recipes.size()-CSYS_ITEM_CNT/2-1));
	pxprint(10, 60, "FPS=" + ToString(getfps()) + " topCur=" + ToString(p.craft.top_cur));
	/*pxprint(10,10,"preTopCur="+ToString(uid craft_precur)+" Cur="+ToString(p.craft.Cur()));
	if(p.craft.Cur() < p.craft.active_recipes.size())
		pxprint(10,170,"curRecipe="+ToString(p.craft.active_recipes.at(p.craft.Cur())));
	pxprint(10,110,"p.json="+p.json.ToString());
	pxprint(10,160,"waitforRelease="+ToString(p.craft.waitForRelease));*/
	//pxprint(10, 110, "espawners.size==" + ToString(world.espawners.size()));
	pxprint(10, 110, "entities.size==" + ToString(entities.size()));
	if (!OuttaWorld(p.x,p.y))
		pxprint(10, 160, "biome=" + biomedata.at(world.GetBlock(p.x, p.y, "DrawDebugInfo").biome).cn_name);
	pxprint(10, 210, "loadedChunks=" + ToString(world.manager.chunks.size()));
	if (!OuttaWorld(p.x, p.y))
		pxprint(10, 260, "cx=" + ToString(ToChunkCoord(p.x)) + " cy=" + ToString(ToChunkCoord(p.y)));
}

void World::UpdateEntitySpawners()
{
	XCPT_L
	if (OuttaWorld(p.x, p.y))	return;
	bool has = false;
	size_t i = 0;
	for (; i < espawners.size(); ++i)
	{
		if (espawners.at(i).biome_id == world.GetBlock(p.x, p.y, "World::UpdateEntitySpawners").biome)
		{
			has = true;
			break;
		}
	}
	if (has)
	{
		espawners.at(i).lastPlayerExit = clock();
	}
	else {
		EntitySpawner espawner;
		espawner.biome_id = world.GetBlock(p.x, p.y, "World::UpdateEntitySpawners").biome;
		auto esdata = biomedata.at(espawner.biome_id).espawn_data;
		for (size_t j = 0; j < esdata.size(); ++j)
		{
			espawner.espawns.push_back(esdata.at(j).Start());
		}
		espawners.push_back(espawner);
	}//!!!

	for (size_t j = 0; j < espawners.size(); ++j)
	{
		if (espawners.at(j).TimeToClose())
		{
			auto iter = espawners.begin();
			long k = 0;
			while (iter != espawners.end() && k != j) {
				iter++;
				k++;
			}
			iter = espawners.erase(iter);
			continue;
		}
	}
	XCPT_R
}
const short _nxtAround[4][2] = {{1,0},{0,1},{-1,0},{0,-1}};
void bfsRenderWorld(BIOME_ID biome, long cnt, long x, long y) {
	if(cnt <= 0 || world.GetBlock(x,y,"brw").biome == biome)
		return;		//剪掉顶芽

	BLOCK_ID bid = GetBiomeRandomBlock(biome);
	world.SetBlock(x,y,bid,biome);

	long _x,_y;
	for(int i=0; i<4; i++) {
		_x=x,_y=y;
		_x += _nxtAround[i][0];
		_y += _nxtAround[i][1];
		if(_x < 0 || _x >= world.manager.map_w || _y < 0 || _y >= world.manager.map_h)
			continue;		//越界剪枝

		bfsRenderWorld(biome,--cnt,_x,_y);	//递归之
	}
	return;
}
short bfs_giveup_percentage = 10;
void bfsRenderWorld_unstable(BIOME_ID biome, long cnt, long x, long y) {
	if(cnt <= 0 || world.GetBlock(x,y,"brwu").biome == biome)
		return;		//剪掉顶芽

	BLOCK_ID bid = GetBiomeRandomBlock(biome);
	world.SetBlock(x,y,bid,biome);

	long _x,_y;
	for(int i=0; i<4; i++) {
		if(Percent(bfs_giveup_percentage))
			continue;
		_x=x,_y=y;
		_x += _nxtAround[i][0];
		_y += _nxtAround[i][1];
		if(_x < 0 || _x >= world.manager.map_w || _y < 0 || _y >= world.manager.map_h)
			continue;		//越界剪枝

		bfsRenderWorld_unstable(biome,--cnt,_x,_y);	//递归之
	}
	return;
}

#define DEF_MAIN_PATH_DIR_WEIGHT 6
int main_path_dir_weight = DEF_MAIN_PATH_DIR_WEIGHT;
#define MAIN_PATH_DIR_WEIGHT main_path_dir_weight
void pathCreater(BLOCK_ID bid, DIR dir, long cnt, long x, long y) {
	//小径开创者 (单向)
	if(x < 0 || x >= world.manager.map_w || y < 0 || y >= world.manager.map_h)
		return;		//越界剪枝
	if(cnt <= 0 || world.GetBlock(x,y,"pathCreater").id == bid)
		return;				//剪枝2

	world.SetBlock(x,y,bid);

	DIR face;

	face = dir;

	if(dir == RIGHT) {
		if(Percent(main_path_dir_weight))
			face = dir;
		else {
			face = Choice({RIGHTDOWN,RIGHTUP});
		}
	} 
	else if(dir == DOWN) {
		if(Percent(main_path_dir_weight))
			face = dir;
		else {
			face = Choice({RIGHTDOWN,LEFTDOWN});
		}
	}else if(dir == LEFT) {
		if(Percent(main_path_dir_weight))
			face = dir;
		else {
			face = Choice({LEFTDOWN,LEFTUP});
		}
	}
	else if(dir == UP) {
		if(Percent(main_path_dir_weight))
			face = dir;
		else {
			face = Choice({RIGHTUP,LEFTUP});
		}
	}else if(dir == RIGHTUP) {
		if(Percent(main_path_dir_weight))
			face = dir;
		else {
			face = Choice({RIGHT,UP});
		}
	}else if(dir == RIGHTDOWN) {
		if(Percent(main_path_dir_weight))
			face = dir;
		else {
			face = Choice({RIGHT,DOWN});
		}
	}else if(dir == LEFTDOWN) {
		if(Percent(main_path_dir_weight))
			face = dir;
		else {
			face = Choice({LEFT,DOWN});
		}
	}else if(dir == LEFTUP) {
		if(Percent(main_path_dir_weight))
			face = dir;
		else {
			face = Choice({UP,LEFT});
		}
	}
	DirOffsetPos(x,y,face);
	pathCreater(bid,face,--cnt,x,y);
}
void SpawnNaturalOres()
{	//生成自然矿石 
    XCPT_L
	repeat(RandomRange(220,366))
	{	//石
		long x,y;
		do{
			x = RandomRange(30,world.manager.map_w-30);
			y = RandomRange(30,world.manager.map_h-30);
		}while(world.GetBlock(x,y,"SpawnNaturalOres").id != 4 && world.GetBlock(x,y,"SpawnNaturalOres").id != 5
		 || OuttaWorld(x,y));
	    ITEM_ID ore = Choice({64,64,64,64,64,64,64,64,64,64,64,64,64,64,62,62,62,62,62,62,60});
	    for(short h = 0; h < Choice({1,2,2,2,2,2,3,3,4,4,5,7,8}); ++h)
	        for(short w = 0; w < Choice({1,2,2,2,2,2,3,3,4,4,5,7,8}); ++w)
	        {
	        	if(OuttaWorld(x+w,y+h) || world.GetBlock(x+w,y+h,"SpawnNaturalOres").id != 4 && world.GetBlock(x+w,y+h,"SpawnNaturalOres").id != 5)
	        	    continue;
	        	if(!world.GetBlock(x+w,y+h,"SpawnNaturalOres").tag.HaveKey("EDrop"))
				{
					string svalue="[";
					if(ore == 64)
					{	//coal
					    if(RandomRange(0,10) < 3)
					        continue;
						svalue += "64s*\"";
						svalue += ToString(Choice({1,1,1,1,1,1,1,1,1,1,1,2}));
						svalue += "\"";
						if(RandomRange(0,10) < 3)	//概率掉落 
						    svalue += "*0.5f";
					}else if(ore == 62)
					{	//iron
					    if(RandomRange(0,10) < 4)
					        continue;
	                    if(RandomRange(0,10) > 2)
	                    {
	                    	svalue += "62s*\"";
	                    	svalue += ToString(Choice({1,1,1,1,1,1,1,1,1,1,1,2,2,3,3,4,7}));
	                    	svalue += "\"";
						}
					    else
					    {
					    	svalue += "63s*\"";
	                    	svalue += ToString(Choice({1,1,1,1,1,1,1,1,1,1,1,2,4}));
	                    	svalue += "\"";
						}
						if(RandomRange(0,10) < 1)	//概率掉落 
						    svalue += "*0.5f";
					}else if(ore == 60)
					{	//gold
					    if(RandomRange(0,10) < 6)
					        continue;
	                    if(RandomRange(0,10) > 2)
	                    {
	                    	svalue += "60s*\"";
	                    	svalue += ToString(Choice({1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,2,3,4,15}));
	                    	svalue += "\"";
						}
						if(RandomRange(0,10) < 5)	//概率掉落 
						    svalue += Choice({"*0.1f","*0.3f","*0.5f","*0.7f","*0.9f"});
					}else{
						break;
					}
					svalue += "]";
					world.GetBlock(x+w,y+h,"SpawnNaturalOres").tag.AddItem("EDrop",svalue);
					world.GetBlock(x+w,y+h,"SpawnNaturalOres").AdjustShape();
				}
			}
		
	}
	repeat(RandomRange(130,266))
	{	//沙里有金粒 
		long x,y;
		do{
			x = RandomRange(30,world.manager.map_w-30);
			y = RandomRange(30,world.manager.map_h-30);
		}while(world.GetBlock(x,y,"SpawnNaturalOres").id != 6
		 || OuttaWorld(x,y));
	    if(!world.GetBlock(x,y,"SpawnNaturalOres").tag.HaveKey("EDrop"))
		{
			string svalue="[";
			svalue += "60s*\"";
			svalue += ToString(Choice({1,1,1,1,1,1,1,1,1,2}));
			svalue += "\"";
			if(RandomRange(0,10) < 6)
			    svalue += "*0.8f";
            svalue += "]";
			world.GetBlock(x,y,"SpawnNaturalOres").tag.AddItem("EDrop",svalue);
			world.GetBlock(x,y,"SpawnNaturalOres").AdjustShape();
		}
	}
	XCPT_R
}
#define OCEAN_WIDTH 30
void SpawnLongWalls(void)
{
	int rx,ry;
	//#2生成随机长城
	main_path_dir_weight = 3;
	//①木栅栏
	repeat(RandomRange(55,150)) {
		rx = RandomRange(OCEAN_WIDTH+20,world.manager.map_w- OCEAN_WIDTH-20,false,false);
		ry = RandomRange(OCEAN_WIDTH+20,world.manager.map_h- OCEAN_WIDTH-20,false,false);
		pathCreater(42,RandomDir4(),RandomRange(30,99),rx,ry);
	}
	main_path_dir_weight = 1;
	//②石砖
	repeat(RandomRange(10,27)) {
		rx = RandomRange(OCEAN_WIDTH+20,world.manager.map_w- OCEAN_WIDTH-20,false,false);
		ry = RandomRange(OCEAN_WIDTH+20,world.manager.map_h- OCEAN_WIDTH-20,false,false);
		pathCreater(22,RandomDir4(),RandomRange(99,67),rx,ry);
	}
	//③木头
	repeat(RandomRange(20,40)) {
		rx = RandomRange(OCEAN_WIDTH+20,world.manager.map_w- OCEAN_WIDTH-20,false,false);
		ry = RandomRange(OCEAN_WIDTH+20,world.manager.map_h- OCEAN_WIDTH-20,false,false);
		pathCreater(13,RandomDir4(),RandomRange(36,86),rx,ry);
	}
	//④铁栏杆 
	main_path_dir_weight = 7;
	repeat(RandomRange(40,70)) {
		rx = RandomRange(OCEAN_WIDTH+20,world.manager.map_w- OCEAN_WIDTH-20,false,false);
		ry = RandomRange(OCEAN_WIDTH+20,world.manager.map_h- OCEAN_WIDTH-20,false,false);
		pathCreater(45,RandomDir4(),RandomRange(25,50),rx,ry);
	}
	//⑤金栏杆 
	repeat(RandomRange(4,14)) {
		rx = RandomRange(OCEAN_WIDTH+30,world.manager.map_w- OCEAN_WIDTH-20,false,false);
		ry = RandomRange(OCEAN_WIDTH+30,world.manager.map_h- OCEAN_WIDTH-20,false,false);
		pathCreater(46,RandomDir4(),RandomRange(10,30),rx,ry);
	}
	main_path_dir_weight = 6;
	//⑥仙人掌篱笆 
	repeat(RandomRange(16,44)) {
		rx = RandomRange(OCEAN_WIDTH+20,world.manager.map_w- OCEAN_WIDTH-20,false,false);
		ry = RandomRange(OCEAN_WIDTH+20,world.manager.map_h- OCEAN_WIDTH-20,false,false);
		pathCreater(72,RandomDir4(),RandomRange(12,80),rx,ry);
	}
}
void SpawnAloneBlocks()
{	//生成一些单独的方块
	repeat(RandomRange(10, 20))
	{
		world.SetBlock(RandomRange(OCEAN_WIDTH+90, world.manager.map_w - OCEAN_WIDTH - 90), RandomRange(10, world.manager.map_h-10), CnNameToBlock("山砧"), biome_void, "SpawnAloneBlocks");
		world.SetBlock(RandomRange(OCEAN_WIDTH + 90, world.manager.map_w - OCEAN_WIDTH - 90), RandomRange(10, world.manager.map_h - 10), CnNameToBlock("粉色花架"), biome_void, "SpawnAloneBlocks");
		world.SetBlock(RandomRange(OCEAN_WIDTH + 90, world.manager.map_w - OCEAN_WIDTH - 90), RandomRange(10, world.manager.map_h - 10), CnNameToBlock("绿叶架"), biome_void, "SpawnAloneBlocks");
		world.SetBlock(RandomRange(OCEAN_WIDTH + 90, world.manager.map_w - OCEAN_WIDTH - 90), RandomRange(10, world.manager.map_h - 10), CnNameToBlock("宝藏幼株"), biome_void, "SpawnAloneBlocks");
		world.SetBlock(RandomRange(OCEAN_WIDTH + 90, world.manager.map_w - OCEAN_WIDTH - 90), RandomRange(10, world.manager.map_h - 10), CnNameToBlock("烽火台"), biome_void, "SpawnAloneBlocks");
	}
	repeat(RandomRange(5, 12))
	{
		long x, y;
		long c = 0;
		do {
			x = RandomRange(OCEAN_WIDTH, world.manager.map_w - OCEAN_WIDTH);
			y = RandomRange(OCEAN_WIDTH, world.manager.map_h - OCEAN_WIDTH);
			c++;
		} while (world.GetBlock(x, y, "SpawnAloneBlocks").biome != biome_desert && c < 1000);
		world.SetBlock(x, y, CnNameToBiome("墓碑树"), biome_void, "SpawnAloneBlocks");
	}
}
void RandomSpreadItemsInChest(Block& bk, const vector<Item>& items)
{
	XCPT_L
	int w = blockdata.at(bk.id).GetContainerWidth();
	int h = blockdata.at(bk.id).GetContainerHeight();
	int sepMaxCnt = w*h - items.size();

	//world.UpdateInspectedContainer(bk);
	//p.inspblock = &bk;
	vector<Item> chest;
	for (size_t i = 0; i < w*h; ++i)
		chest.push_back(Item());
	//DebugLog("item.size=" + ToString(items.size()));
	for (size_t i = 0; i < items.size(); ++i)
	{
		//DebugLog("i=" + ToString(i));
		const Item& item = items.at(i);
		int x=0, y=0;
		if (item.cnt == 1)
		{
			//DebugLog("cnt=1");
			int r = 1;
			do {
				x = RandomRange(0, w - 1);
				y = RandomRange(0, h - 1);
				if (r > 10000)
					break;
				++r;
			} while (chest.at(y*w+x).id != 0);
			chest.at(y*w + x).Assign(item);
			//DebugLog("ok");
		}
		else {
			int remainCnt = item.cnt;
			while (remainCnt > 0)
			{
				//DebugLog("remainCnt=" + ToString(remainCnt));
				int r = 1;
				do {
					x = RandomRange(0, w - 1);
					y = RandomRange(0, h - 1);
					if (r > 10000)
						break;
					++r;
				} while (chest.at(y*w + x).id != 0);
				chest.at(y*w + x).id = item.id;
				//DebugLog("id=" + ToString(item.id));
				if (sepMaxCnt > 0)
					chest.at(y*w + x).cnt = RandomRange(Clamp<int>(item.cnt*0.1, 1, remainCnt), Clamp<int>(item.cnt*0.4, 1, remainCnt));
				else
					chest.at(y*w + x).cnt = remainCnt;
				remainCnt -= chest.at(y*w + x).cnt;
				--sepMaxCnt;
				//DebugLog("sepMaxCnt=" + ToString(sepMaxCnt));
			}
			/*int r = 1;
			do {
				x = RandomRange(0, w - 1);
				y = RandomRange(0, h - 1);
				if (r > 10000)
					break;
				++r;
			} while (chest.at(y*w + x).id != 0);
			chest.at(y*w + x).Assign(item);
			*/
		}
		
	}
	//DebugLog("saving");
	//world.SaveInspectedContainer(*p.inspblock);
	//world.insp_container.clear();	//释放内存 
	//p.inspblock = nullptr;
	SaveContainer(bk, chest);
	//DebugLog("ok");
	XCPT_R
}
#define SCL_EMPTY 0
#define SCL_RUIN 1
void SpawnChestLoot(Block& bk, UINT uWhere)
{
	XCPT_L
	if (uWhere == SCL_EMPTY)	return;
	Item majorLoot;
	majorLoot.cnt = 1;
	vector<Item> minorLoot;
	vector<pair<ITEM_ID, string>> minorLootChoices;
	short minorLootCnt;
	if (uWhere == SCL_RUIN)
	{
		minorLootCnt = RandomRange(3, 5);
		majorLoot.id = Choice({49, 51, 52, 52, 53, 53, 54, 54, 55, 103, 103, 104, 106, 113, 170, 206, 206, 207});
		majorLoot.AdaptDurability();
		minorLootChoices = {
			make_pair(13, "2~4"),	//log
			make_pair(45, "3~6"),	//stick
			make_pair(45, "3~6"),
			make_pair(58, "1~6"),	//iron nugget
			make_pair(59, "0~2"),	//iron ingot
			make_pair(60, "0~2"),	//gold nugget
			make_pair(61, "0~1"),	//gold ingot
			make_pair(62, "3~7"),	//small iron ore
			make_pair(63, "0~3"),	//iron ore
			make_pair(64, "3~9"),	//coal
			make_pair(64, "3~9"),
			make_pair(65, "4~10"),	//charcoal
			make_pair(67, "3~8"),	//torch
			make_pair(107, "2~5"),	//string
			make_pair(108, "1~3"),	//rope
			make_pair(109, "6~19"),	//wooden arrow
			make_pair(111, "5~20"),	//bamboo arrow
			make_pair(114, "1~4"),	//powder
			make_pair(116, "0~1"),	//spring
			make_pair(140, "1~2"),	//cloth
			make_pair(144, "3~7"),	//bone
			make_pair(144, "3~7"),	
			make_pair(144, "3~7"),
			make_pair(159, "1~4"),	//apple
			make_pair(161, "1~4"),	//pear
			make_pair(186, "1~3"),	//paper
			make_pair(205, "3~6"),	//throwable spear
			make_pair(218, "1~3"),	//potato
			make_pair(220, "1~3"),	//feather
			make_pair(221, "2~5"),	//wheat
			make_pair(227, "1~6"),	//throwing knife
			make_pair(228, "0~1"),	//toxin
		};
	}
	else {
		ErrorLogTip(NULL, "无效的宝箱战利品刷新地点：" + ToString(uWhere));
		return;
	}
	for (size_t i = 0; i < minorLootCnt; ++i)
	{
		auto choice = Choice(minorLootChoices);
		minorLoot.emplace_back(choice.first, RangeChoice(choice.second), "{}");
	}
	minorLoot.insert(begin(minorLoot), majorLoot);
	RandomSpreadItemsInChest(bk, minorLoot);
	XCPT_R
}
void SpawnRuins(void) {	//生成遗迹
	//#1 生成天然小房间
	XCPT_L
	const vector<BLOCK_ID> wall_list	//墙体预设
	{4/*Stone*/,4,13/*log*/,14/*plank*/,14,14,20/*ice*/,22/*stonebrick*/,22,23/*hellstonebrick*/,};
	int wall,x,y;
	repeat(RandomRange(120,240,true,true)) {
		//$定位
		do {
			x=RandomRange(40,world.manager.map_w-60,true,true);
			y=RandomRange(50,world.manager.map_h-60,true,true);
		} while(world.GetBlock(x,y,"SpawnRuins").biome == biome_ocean );
		//$选墙
		wall=wall_list.at(RandomRange(0,wall_list.size(),true,false));
		//$布局预设定
		BIOME_ID bm = world.GetBlock(x,y,"SpawnRuins").biome;

		bool bChest=false;	//是否宝箱ed？
		bool bDoor =false;	//是否装了门？
		UINT fnt_rest=RandomRange(1,3,true,true);	//家具数量
		const vector<BLOCK_ID> fnt_list	//家具预设
		//<!>BUG：最后一个取不到
		{15/*workbench*/,15,15,15,15,15,15,15,15,15,28/*furnace*/,28,28,28,28,28,29/*anvil*/,29,31/*campfire*/,31,47,47,47,47,48,48,49,50,51,52,53,53,53,54,54,55,56,57,57,57,57,58,58,59,60,61,61,62,62,62,71,76		};
		int fnt;	//家具
		//$筑造
		int rx,ry;	//尺寸
		rx=RandomRange(6,12,true,true);
		ry=RandomRange(6,14,true,true);
		int i,j;
		UINT doorIndex=RandomRange(1,(rx*ry-(rx-2)*(ry-2)-4),true,false),index=0;
		UINT chestIndex=RandomRange(1,((rx-2)*(ry-2)-1),true,false),cIndex=0;
		//↑必定会抽到一个墙 不怕没门了
		for(i=0; i<ry; i++) {

			for(j=0; j<rx; j++) {
				if(i==0||i==ry-1||j==0||j==rx-1) {
					//壁
					world.SetBlock(x+j,y+i,wall);
					if((i==0&&(j==0||j==rx-1)) || (i==ry-1&&(j==0||j==rx-1)))//四角
						continue;
					index++;
					if(doorIndex == index) {
						BLOCK_ID door;
						if(wall == 4 || wall == 22)//stone
							door = 38;
						else if(wall == 13 || wall == 14)	//wood
							door = 37;
						else if(wall == 20)	//ice
							door = 39;
						else if(wall == 23)	//hell_stonebrick
							door = 41;	//hell_door
						else
							door = 40;	//golden door

						int r=RandomRange(0,10,true,false);
						string btag="{Open:"+ToString(r>3?"1s":"0s")+"}";
//					string btag="{Open:1s}";
						world.SetBlock(x+j,y+i,door,biome_void,"SpawnRuins",btag.c_str());
					}
					continue;
				} else
					cIndex++;
				world.SetBlock(x+j,y+i,0);

				if(chestIndex == cIndex) { //箱
					world.SetBlock(x+j,y+i,Choice({32,32,33,33,34,34,35,36}));
					SpawnChestLoot(world.GetBlock(x + j, y + i, "SpawnRuins"), SCL_RUIN);
//				chest.RoomRandomChest(bm);//random
					bChest = true;
					continue;
				}
				int r=RandomRange(0,100,true,false);
				if(r > 80 && fnt_rest > 0) {	//家具
					fnt = Choice(fnt_list);
					fnt_rest--;
					world.SetBlock(x+j,y+i,fnt);
					continue;
				}
			}
			j=0;
		}
	}
	XCPT_R
}
void DensityAreaSetBlock(int rpt_min,int rpt_max, int spr_min, int spr_max, int den_min, int den_max, BLOCK_ID id)
{
	XCPT_L
	repeat(RandomRange(rpt_min,rpt_max))
	{
			int rx=RandomRange(200,world.manager.map_w-200,true,true);
			int ry=RandomRange(200,world.manager.map_h-200,true,true);
			int dx=RandomRange(spr_min,spr_max,true,true);
			int dy=RandomRange(spr_min,spr_max,true,true);
			
			int density = RandomRange(den_min,den_max);	//密度 
			
			for(int _y=ry;_y <= ry+dy;_y++)
			   for(int _x=rx;_x<=rx+dx;_x++)
			   {
			   	       int r=RandomRange(0,100,true,false);
			   		   if(r < density 
						  && world.GetBlock(_x,_y,"DensityAreaSetBlock").biome != biome_ocean
						  && world.GetBlock(_x,_y,"DensityAreaSetBlock").id != 17)
			   		   {
			   		   		world.SetBlock(_x, _y, id, biome_void);
				       }
			   }
	}
	XCPT_R
}
void SpawnPlants()
{
	XCPT_L
	DensityAreaSetBlock(3, 6, 6,35,20,85,EnNameToBlock("reeds"));	//芦苇 
	DensityAreaSetBlock(3, 5, 4, 25, 9, 65, EnNameToBlock("sugar_cane"));	//甘蔗
	DensityAreaSetBlock(30,60,10,30,2,11,EnNameToBlock("pink_flower"));	//粉色花 
	DensityAreaSetBlock(22,40,17,40,19,36,EnNameToBlock("scarlet_grass"));	//猩红草 
	DensityAreaSetBlock(9,19,4,30,3,8,EnNameToBlock("golden_flower"));	//金色花 
	DensityAreaSetBlock(10,13,34,45,4,16,EnNameToBlock("blue_grass"));	//碧蓝草 
	DensityAreaSetBlock(24,33,34,55,3,7,EnNameToBlock("ice_lotus"));	//冰雪莲 
	DensityAreaSetBlock(19, 24, 8, 13, 8, 10, EnNameToBlock("carrot_plant"));	//萝卜
	DensityAreaSetBlock(19, 24, 8, 13, 8, 10, EnNameToBlock("potato_plant"));	//马铃薯
	DensityAreaSetBlock(15, 20, 14, 43, 35, 75, EnNameToBlock("wheat_plant"));	//小麦
	XCPT_R
}
inline bool AllowAnimalSpawnBiome(BIOME_ID bid)
{
	return (bid != biome_ocean && bid != biome_void && bid != biome_dungeon && bid != biome_lake && bid != biome_cave
		&& bid != biome_iceland);
}
void AreaCreateAnimals(ENTITY_ID eid)
{
	int rx, ry;
_aca:
	rx = RandomRange(OCEAN_WIDTH, world.manager.map_w - OCEAN_WIDTH, true, true);
	ry = RandomRange(OCEAN_WIDTH, world.manager.map_h - OCEAN_WIDTH, true, true);

	if (!AllowAnimalSpawnBiome(world.GetBlock(rx,ry,"AreaCreateAnimals").biome))
		goto _aca;//retry

	int dx = RandomRange(10, 60, true, true);
	int dy = RandomRange(10, 60, true, true);
	int density = Choice({1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,2,2,2,2,2,2,2,2,2,2,2,2,2,2,3,3,3,3,3,3,3,4,5});	//density
	for (int _y = ry; _y <= ry + dy; _y++)
		for (int _x = rx; _x <= rx + dx; _x++)
		{
			if (OuttaWorld(_x, _y))		continue;
			int r = RandomRange(0, 100, true, false);
			if (r < density && AllowAnimalSpawnBiome(world.GetBlock(_x, _y, "AreaCreateAnimals").biome)
				&& !world.GetBlock(_x,_y,"AreaCreateAnimals").IsBarrier()
				&& !ExistEntityHere(_x,_y))
			{
				SpawnEntity(eid, _x, _y, RandomDir4());
			}
		}
}
void SpawnNaturalAnimals()
{
	repeat(RandomRange(40, 60))
		AreaCreateAnimals(27);//cow
	repeat(RandomRange(25, 50))
		AreaCreateAnimals(28);//sheep
	repeat(RandomRange(45, 70))
		AreaCreateAnimals(29);//pig
	repeat(RandomRange(20, 55))
		AreaCreateAnimals(31);//horse
	repeat(RandomRange(50, 75))
		AreaCreateAnimals(30);//chicken
}

#define NV_DELAY_MS 100
#define nvde api_sleep(NV_DELAY_MS);
DWORD ThrCreateNewVoxel(LPVOID none) {
	XCPT_L
	DebugLog("[ThrCreateNewVoxel] 开始创建维度");
	g_nv_status=0;
	DebugLog("[ThrCreateNewVoxel] 随机地图大小...");

	world.Clear();
	world.manager.map_w = RandomRange(MAP_X_MIN/CHUNK_PX,MAP_X_MAX/ CHUNK_PX)*CHUNK_PX;
	world.manager.map_h = RandomRange(MAP_Y_MIN/ CHUNK_PX,MAP_Y_MAX/ CHUNK_PX)*CHUNK_PX;
	world.time = World::Morning;
	api_sleep(500);	
	
	g_nv_status=1;
	DebugLog("[ThrCreateNewVoxel] 生成缺省地形...");
	world.manager.InitAllChunks(true);	//初始化并保留
	//													  所有区块
	for(int _y = 0; _y < world.manager.map_h; _y ++)
		for(int _x = 0; _x < world.manager.map_w; _x ++) {
			Block b;
			BLOCK_ID _id = 0;
			short r = RandomRange(0,10);
			if(r < 3)
				_id = 1;
			else if(r < 4)
				_id = 2;
			else if(r < 7)
				_id = 0;
			else if(r < 8)
				_id = 7;
			else
				_id = 5;
			b.id = _id;
			b.biome = biome_pine_forest;
			b.dur = blockdata.at(b.id).dur;
			b.AdjustShape();
			world.manager.GetBlock(_x, _y) = b;
			//world.blocks.back().biome = biome_pine_forest;
		}
	DebugLog("[ThrCreateNewVoxel] 生成沙漠...");
	g_nv_status = 2;

	for(short j = 0; j < RandomRange(6,12); ++j) {
		long _x,_y;
		_x = RandomRange(10,world.manager.map_w-10);
		_y = RandomRange(10,world.manager.map_h-10);
		bfsRenderWorld(biome_desert,RandomRange(100,300),_x,_y);
	}
	nvde
	DebugLog("[ThrCreateNewVoxel] 生成松树林...");
	g_nv_status = 3;

	for(short j = 0; j < RandomRange(9,19); ++j) {
		long _x,_y;
		_x = RandomRange(10,world.manager.map_w-10);
		_y = RandomRange(10,world.manager.map_h-10);
		bfsRenderWorld(biome_pine_forest,RandomRange(40,145),_x,_y);
	}
	nvde
	DebugLog("[ThrCreateNewVoxel] 生成杨树林...");
	g_nv_status = 4;

	for(short j = 0; j < RandomRange(4,9); ++j) {
		long _x,_y;
		_x = RandomRange(30,world.manager.map_w-30);
		_y = RandomRange(30,world.manager.map_h-30);
		bfsRenderWorld(biome_poplar_forest,RandomRange(40,133),_x,_y);
	}
	nvde
	DebugLog("[ThrCreateNewVoxel] 生成银杏林...");
	g_nv_status = 5;

	for(short j = 0; j < RandomRange(2,6); ++j) {
		long _x,_y;
		_x = RandomRange(30,world.manager.map_w-30);
		_y = RandomRange(30,world.manager.map_h-30);
		bfsRenderWorld(biome_ginkgo_forest,RandomRange(63,165),_x,_y);
	}
	nvde
	DebugLog("[ThrCreateNewVoxel] 生成沼泽...");
	g_nv_status = 6;
	bfs_giveup_percentage = 20;

	for(short j = 0; j < RandomRange(5,8); ++j) {
		long _x,_y;
		_x = RandomRange(5,world.manager.map_w-5);
		_y = RandomRange(5,world.manager.map_h-5);
		bfsRenderWorld_unstable(biome_swamp,RandomRange(89,487),_x,_y);
	}
	nvde
	DebugLog("[ThrCreateNewVoxel] 生成樟树林...");
	g_nv_status = 7;

	for(short j = 0; j < RandomRange(5,11); ++j) {
		long _x,_y;
		_x = RandomRange(60,world.manager.map_w-60);
		_y = RandomRange(60,world.manager.map_h-60);
		bfsRenderWorld(biome_camphor_forest,RandomRange(90,133),_x,_y);
	}
	nvde
	DebugLog("[ThrCreateNewVoxel] 生成岩壁...");
	g_nv_status = 8;
	bfs_giveup_percentage = 3;

	for(short j = 0; j < RandomRange(9,17); ++j) {
		long _x,_y;
		_x = RandomRange(5,world.manager.map_w-5);
		_y = RandomRange(5,world.manager.map_h-5);
		bfsRenderWorld_unstable(biome_stones,RandomRange(120,444),_x,_y);
	}
	nvde
	DebugLog("[ThrCreateNewVoxel] 生成石林...");
	g_nv_status = 9;
	bfs_giveup_percentage = 10;

	for(short j = 0; j < RandomRange(5,7); ++j) {
		long _x,_y;
		_x = RandomRange(5,world.manager.map_w-5);
		_y = RandomRange(5,world.manager.map_h-5);
		bfsRenderWorld_unstable(biome_rocks,RandomRange(60,177),_x,_y);
	}
	nvde
	DebugLog("[ThrCreateNewVoxel] 生成草原...");
	g_nv_status = 10;

	for(short j = 0; j < RandomRange(6,9); ++j) {
		long _x,_y;
		_x = RandomRange(5,world.manager.map_w-5);
		_y = RandomRange(5,world.manager.map_h-5);
		bfsRenderWorld(biome_grass,RandomRange(100,244),_x,_y);
	}
	nvde
	DebugLog("[ThrCreateNewVoxel] 生成丛林...");
	g_nv_status = 11;

	for(short j = 0; j < RandomRange(5,11); ++j) {
		long _x,_y;
		_x = RandomRange(55,world.manager.map_w-55);
		_y = RandomRange(55,world.manager.map_h-55);
		bfsRenderWorld(biome_jungle,RandomRange(352,744),_x,_y);
	}
	nvde
	DebugLog("[ThrCreateNewVoxel] 生成竹林...");
	g_nv_status = 12;
	bfs_giveup_percentage = 20;

	for(short j = 0; j < RandomRange(6,8); ++j) {
		long _x,_y;
		_x = RandomRange(55,world.manager.map_w-55);
		_y = RandomRange(55,world.manager.map_h-55);
		bfsRenderWorld_unstable(biome_bamboo_forest,RandomRange(90,297),_x,_y);
	}
	nvde
	DebugLog("[ThrCreateNewVoxel] 生成冰原...");
	g_nv_status = 13;

	for(short j = 0; j < RandomRange(5,10); ++j) {
		long _x,_y;
		_x = RandomRange(55,world.manager.map_w-55);
		_y = RandomRange(55,world.manager.map_h-55);
		bfsRenderWorld(biome_iceland,RandomRange(99,346),_x,_y);
	}
	nvde
	DebugLog("[ThrCreateNewVoxel] 生成针叶林...");
	g_nv_status = 14;

	for(short j = 0; j < RandomRange(8,14); ++j) {
		long _x,_y;
		_x = RandomRange(5,world.manager.map_w-5);
		_y = RandomRange(5,world.manager.map_h-5);
		bfsRenderWorld(biome_taiga,RandomRange(333,666),_x,_y);
	}
	nvde
	DebugLog("[ThrCreateNewVoxel] 生成果树林...");
	g_nv_status = 15;

	for(short j = 0; j < RandomRange(4,8); ++j) {
		long _x,_y;
		_x = RandomRange(5,world.manager.map_w-5);
		_y = RandomRange(5,world.manager.map_h-5);
		bfsRenderWorld(biome_apple_forest,RandomRange(90,444),_x,_y);
	}
	for(short j = 0; j < RandomRange(5,7); ++j) {
		long _x,_y;
		_x = RandomRange(25,world.manager.map_w-25);
		_y = RandomRange(25,world.manager.map_h-25);
		bfsRenderWorld(biome_pear_forest,RandomRange(104,380),_x,_y);
	}
	
	nvde
	DebugLog("[ThrCreateNewVoxel] 生成山区...");
	g_nv_status = 16;
	bfs_giveup_percentage = 40;

	for(short j = 0; j < RandomRange(22,43); ++j) {
		long _x,_y;
		_x = RandomRange(25,world.manager.map_w-25);
		_y = RandomRange(35,world.manager.map_h-35);
		bfsRenderWorld_unstable(biome_mountain,RandomRange(34,67),_x,_y);
	}
	nvde
	DebugLog("[ThrCreateNewVoxel] 埋藏矿石...");
	g_nv_status = 17;
	SpawnNaturalOres();
	
	nvde
	DebugLog("[ThrCreateNewVoxel] 生成小路...");
	g_nv_status = 18;
	for(short j = 0; j < RandomRange(3900,6400); ++j) {
		long _x,_y;
		_x = RandomRange(5,world.manager.map_w-5);
		_y = RandomRange(5,world.manager.map_h-5);
		pathCreater(0,RandomDir4(),RandomRange(10,320),_x,_y);
	}
	nvde
	SpawnLongWalls();
	SpawnAloneBlocks();
	
	nvde
	DebugLog("[ThrCreateNewVoxel] 生成溪流...");
	g_nv_status = 19;
	main_path_dir_weight = 4;
	for(short j = 0; j < RandomRange(360,600); ++j) {
		long _x,_y;
		_x = RandomRange(5,world.manager.map_w-5);
		_y = RandomRange(5,world.manager.map_h-5);
		pathCreater(EnNameToBlock("water"),RandomDir4(),RandomRange(11,56),_x,_y);
	}
	nvde
	DebugLog("[ThrCreateNewVoxel] 生成河流...");
	g_nv_status = 20;
	for(short j = 0; j < RandomRange(120,230); ++j) {
		long _x,_y;
		main_path_dir_weight = Choice({8,8,8,2,5,7,9});
		_x = RandomRange(65,world.manager.map_w-65);
		_y = RandomRange(65,world.manager.map_h-65);
		pathCreater(EnNameToBlock("water"),RandomDir4(),RandomRange(110,760),_x,_y);
	}
	main_path_dir_weight = 9;
	for(short j = 0; j < RandomRange(60,90); ++j) {
		long _x,_y;
		main_path_dir_weight = Choice({9,8,8,8,4});
		_x = RandomRange(65,world.manager.map_w-65);
		_y = RandomRange(65,world.manager.map_h-65);
		pathCreater(EnNameToBlock("water"),RandomDir4(),RandomRange(3780,5360),_x,_y);
	}
	nvde
	DebugLog("[ThrCreateNewVoxel] 生成各种植物...");
	g_nv_status = 21;
	SpawnPlants();
	nvde
	long _x,_y;
	main_path_dir_weight = 3;
	_x = RandomRange(65,world.manager.map_w-65);
	_y = RandomRange(65,world.manager.map_h-65);
	pathCreater(EnNameToBlock("water"),RandomDir4(),RandomRange(12780,55360),_x,_y);
	nvde
	DebugLog("[ThrCreateNewVoxel] 生成湖泊...");
	g_nv_status = 22;
	bfs_giveup_percentage = 25;
	for(short j = 0; j < RandomRange(14,28); ++j) {
		long _x,_y;
		_x = RandomRange(25,world.manager.map_w-25);
		_y = RandomRange(35,world.manager.map_h-35);
		bfsRenderWorld_unstable(biome_lake,Choice({8,10,24,33,36,56,91,140}),_x,_y);
	}
	nvde
	DebugLog("[ThrCreateNewVoxel] 生成遗迹...");
	g_nv_status = 23;
	SpawnRuins();

	nvde
	DebugLog("[ThrCreateNewVoxel] 生成自然动物...");
	g_nv_status = 24;
	SpawnNaturalAnimals();

	nvde
	DebugLog("[ThrCreateNewVoxel] 生成海洋...");
	g_nv_status = 25;
	/*for(short j = 0; j < RandomRange(2,4); ++j)
	{
		long _x,_y;
		_x = RandomRange(5,world.manager.map_w-5);
		_y = RandomRange(5,world.manager.map_h-5);
		bfsRenderWorld(biome_ocean,RandomRange(666,1200),_x,_y);
	}*/

	for(long y = 0; y < world.manager.map_h; ++y)
		for(long x = 0; x < world.manager.map_w; ++x) {
			if(x < OCEAN_WIDTH || x >= world.manager.map_w - OCEAN_WIDTH
			        || y < OCEAN_WIDTH || y >= world.manager.map_h - OCEAN_WIDTH) {
//				DebugLog("x="+ToString(x)+" y="+ToString(y));
				world.SetBlock(x,y,EnNameToBlock("water"),biome_ocean,"ThrCreateNewVoxel");
			}
		}
	DebugLog("[ThrCreateNewVoxel] 保存维度至文件...");
	g_nv_status = 26;
	SaveWorld();
	api_sleep(2500);
	DebugLog("[ThrCreateNewVoxel] 完成创建维度");
	g_nv_status=SHRT_MAX;	//Completed
	return 1;
	XCPT_R
}

const char* GetNVCreationProcess(short status) {
	switch(status) {
		case -1:
			return "Creation Preparing...";
		case 0:
			return "Deciding the World size";
		case 1:
			return "Randomly filling the default terrain";
		case 2:
			return "Putting Vast Deserts...";
		case 3:
			return "Spawning Pine Forests...";
		case 4:
			return "Planting Poplar Trees...";
		case 5:
			return "Creating Ginkgo Forests...";
		case 6:
			return "Spawning swamps...";
		case 7:
			return "Making Camphor Forests...";
		case 8:
			return "Creating Stone Walls...";
		case 9:
			return "Preparing Rock Forests...";
		case 10:
			return "Putting Large Grasslands...";
		case 11:
			return "Spawning Jungles...";
		case 12:
			return "Putting Bamboo Forests...";
		case 13:
			return "Creating Icelands...";
		case 14:
			return "Spawning Taiga...";
		case 15:
			return "Planting Various Fruit Trees...";
		case 16:
			return "Making Mountains...";
		case 17:
			return "Hiding Natural Ores...";
		case 18:
			return "Creating pathes...";
		case 19:
			return "Creating streams...";
		case 20:
			return "Creating rivers...";
		case 21:
			return "Spawning Various Plants...";
		case 22:
			return "Pouring Lakes...";
		case 23:
			return "Creating Ruins...";
		case 24:
			return "Spreading Animals...";
		case 25:
			return "Creating Oceans...";
		case 26:
			return "Saving the Voxel";
		default:
			return "*** Mysterious Process ***";
	}
	return nullptr;
}
PIMAGE newVoxelMap=nullptr;
void DrawNewVoxelCreation(bool firstDraw=false) {
	constexpr int text_fs = 65;
	constexpr int proc_fs = 50;
	cls();

	setfont(text_fs,0,"Determination Mono");
	setcolor(YELLOW);
	midpxprint("正在创建新维度",uiy*0.75,0,uix,0,firstDraw?80:0);

	string sProc = GetNVCreationProcess(g_nv_status);
	setfont(proc_fs,0,"Determination Mono");
	setcolor(LIGHTGRAY);
	midpxprint(sProc,uiy*0.75+text_fs+10,0,uix,2,0);
	
	if(newVoxelMap != nullptr)
	{
		putimage(uix*0.5-getwidth(newVoxelMap)/2,uiy*0.2,newVoxelMap);
	}

	delay_fps(60);
}
#define NVMAP_UPDATE_CD 5
#define NVMAP_MAX_W (uix*0.5)
#define NVMAP_MAX_H (uiy*0.5)
void UpdateNewVoxelMap(void)
{
	XCPT_L
	if(g_nv_status < 2)	//no basic structure
		return;
	if(newVoxelMap)
	{
		delimage(newVoxelMap);
		newVoxelMap=nullptr;
	}
	newVoxelMap = PrintedMap();
	//不能太大 
	if(getwidth(newVoxelMap) > NVMAP_MAX_W)
		_resize(newVoxelMap,NVMAP_MAX_W,((float)getheight(newVoxelMap)*NVMAP_MAX_W/getwidth(newVoxelMap)));
	if(getheight(newVoxelMap) > NVMAP_MAX_H)
		_resize(newVoxelMap,((float)getwidth(newVoxelMap)*NVMAP_MAX_H/getheight(newVoxelMap)),NVMAP_MAX_H);
#ifndef NO_NVMAP_BLURRING
	imagefilter_blurring(newVoxelMap,0xFF,0xFF);
#endif
	XCPT_R
}
bool CreateNewVoxel() {
	unsigned long seed=0;
	string pname="";
	DebugLog("准备创建新维度...");
	bool go = EnterNewVoxelCfg(seed,pname);
	if(!go) {
		DebugLog("放弃创建新维度!");
		return false;
	}
	/*midpxprint("种子="+ToString(seed)+"玩家名="+pname,uiy*0.5);
	delay_fps(60);*/
	DebugLog("当前随机数种子："+ToString(seed));
	srand(seed);
	DebugLog("===== 开始创建新维度 =====");

	if(newVoxelMap)
	{
		delimage(newVoxelMap);
		newVoxelMap=nullptr;
	}
	CreateThread((LPTHREAD_START_ROUTINE)ThrCreateNewVoxel);
	delay_fps(60);
	clock_t lastDraw = clock();
	clock_t lastUpdateMap = clock();
	bool firstDraw=true;

	while(is_run()) {
		if(clock() - lastDraw >= NVCFG_DRAW_CD) {
			DrawNewVoxelCreation(firstDraw);
			if(firstDraw)
				firstDraw = false;
			lastDraw = clock();
		}
		if(clock() - lastUpdateMap >= NVMAP_UPDATE_CD)
		{
			UpdateNewVoxelMap();
			lastUpdateMap = clock();
		}
		if(g_nv_status == SHRT_MAX) {
			//DONE
			g_nv_status = -1;
			break;
		}
	}

	cls();
	setfont(65,0,"Determination Mono");
	setcolor(WHITE);
	midpxprint("正在赋予"+pname+"生命",uiy*0.45,0,uix,1,20);
	DebugLog("玩家名为"+pname+"，创建玩家数据中");
	CreateNewPlayer(pname);	//Saving inside
	delay(500);

	cls();
	setcolor(LIGHTGREEN);
	midpxprint("维度构建完成",uiy*0.45,0,uix,1,40);
	delay(1000);
	cls();
	delay_fps(60);
	delay(100);

	return true;
}
void DrawCreationChoice(bool firstDraw=false) {
	cls();

	setfont(55*(uiy/1080.0),0,"Determination Mono");
	setcolor(YELLOW);
	midpxprint("没有找到旧存档。",uiy*0.28,0,uix,2,firstDraw?50:0);
	if(uid ui_play_cc_pre_id == 0)
		setcolor(LIGHTRED);
	else
		setcolor(LIGHTCYAN);
	uid rt_play_create_new = midpxprint("[Space: 开始新篇章]",uiy*0.45,0,uix,0,firstDraw?10:0);
	if(uid ui_play_cc_pre_id == 1)
		setcolor(LIGHTRED);
	else
		setcolor(LIGHTCYAN);
	uid rt_play_giveup = midpxprint("[Esc: 返回]",uiy*0.45+55*2,0,uix,3,firstDraw?7:0);

	delay_fps(60);
}
#define CC_DRAW_CD 60
bool EnterCreationChoice() {
	//返回值为false时代表放弃
	bool firstDraw=true;
	clock_t lastDraw = clock();
	while(is_run()) {
		if(clock() - lastDraw >= CC_DRAW_CD) {
			DrawCreationChoice(firstDraw);
			if(firstDraw)	firstDraw=false;
			lastDraw = clock();
		}
		if(kbmsg()) {
			if(K(VK_SPACE)) {
				SND("click");
				while (K(VK_SPACE));
				return CreateNewVoxel();
			} else if(K(VK_ESCAPE)) {
				SND("click");
				DebugLog("放弃创建新维度。");
				while (K(VK_ESCAPE));
				return false;
			}
		} else if(mousemsg()) {
			mouse_msg msg = getmouse();
			if(InRect(msg.x,msg.y, uid rt_play_create_new)) {
				if(msg.is_left() && msg.is_up()) {
					SND("click");
					return CreateNewVoxel();
				} else
					uid ui_play_cc_pre_id = 0;
			} else if(InRect(msg.x,msg.y, uid rt_play_giveup)) {
				if(msg.is_left() && msg.is_up()) {
					SND("click");
					DebugLog("放弃创建新维度。");
					return false;
				} else
					uid ui_play_cc_pre_id = 1;
			} else
				uid ui_play_cc_pre_id = -1;
		}
	}
	return false;
}
bool ExistSaves(void) {
	return ExistFile(WORLDDATA_PATH);
}
bool ReadPlayerData() {
	if(!players.empty())
		players.clear();
	Player p1;
	players.push_back(p1);

	if(!ExistFile(PLAYERDATA_PATH)) {
		WarnLog("没有发现玩家数据存档！","ReadPlayerData");
		return false;
	}
	DebugLog("【读取】读取玩家数据: "+PLAYERDATA_PATH);
	fstream fin(PLAYERDATA_PATH,ios::in);

	string sVer="";
	string tmp;
	fin>>tmp;

	optinput(fin, sVer);
	if(sVer != string(CURRENT_VERSION)) {
		ErrorLogTip(NULL,"读取玩家数据时发现异常：\n数据版本有差异。存档版本："+sVer,"ReadPlayerData");
		fin.close();
		return false;
	}
	optinput(fin, p.name);
	optinput(fin, p.uuid);
	optinput(fin, p.x);
	optinput(fin, p.y);
	optinput(fin, p.hp);
	optinput(fin, p.max_hp);
	optinput(fin, p.mp);
	optinput(fin, p.max_mp);
	optinput(fin, p.sp);
	optinput(fin, p.max_sp);
	optinput(fin, p.luck);
	optinput(fin, tmp);
	p.achievements = ParseAchievements(tmp);
	fin>>tmp;
	for(short i = 0; i < ITEM_CNT; ++i) {
		string item;
		optinput(fin, item);
		vector<string> itemSep = CutLine(item,'^');
		if(itemSep.size() < 3) {
			ErrorLogTip(NULL,"读取玩家数据存档时出错：\n单物品Item"+ToString(i)+"数据项内有缺失","ReadPlayerData");
			continue;
		}
		p.bag.items[i].Reset(Convert<string,ITEM_ID>(itemSep[0]),
		                     Convert<string,short>(itemSep[1]),
		                     itemSep[2].c_str());
	}

	fin.close();
	return true;
}
void ReadSave() {
	ReadPlayerData();
	ReadWorld();
}
void Save() {
	SavePlayerData();
	SaveWorld();
}
inline string RandomTip() {
	return Choice(tips).text;
}
void DrawTip(const string& tip) {
	setfont(50*(uix/1920.0),0,"Determination Mono");
	setcolor(LIGHTGRAY);
	midpxprint(tip,uiy*0.78);
}
void SinglePlayer() {
	DebugLog("进入单人游戏");
	cls();

	if(!ExistSaves()) {
		bool ret = EnterCreationChoice();
		if(!ret) {	//giveup
			return;
		}
		ReadSave();
	} else {

		cls();

		string tip = RandomTip();

		DrawTip(tip);
		setfont(60,0,"Determination Mono");
		setcolor(YELLOW);
		midpxprint("读取存档中",uiy*0.35,0,uix,0,50);
		ReadSave();
		cls();
		DrawTip(tip);

		setfont(60,0,"Determination Mono");
		setcolor(MAGENTA);
		midpxprint("加入世界中",uiy*0.35,0,uix,0,50);
		delay(1000);

		cls();
	}
	InGame();
}
void Leave() {
	DebugLog("离开游戏...");
	for (int i = 0; i < chn_items.size(); ++i)
		delimage(chn_items.at(i).img);
	chn_items.clear();
	for (int i = 0; i < chn_items_unicode.size(); ++i)
		delimage(chn_items_unicode.at(i).img);
	chn_items_unicode.clear();
	RemoveFontResource((LOCAL_DATA_DIR + "DeterminationMono.otf").c_str());
	delimage(uid white_screen);
	delimage(uid black_screen);
	closegraph();
	exit(0);
}

void SaveOptions() {
	DebugLog("【保存】保存游戏配置中:"+OPTIONS_PATH);
	fstream fout(OPTIONS_PATH,ios::out);
	fout<<"[Options]"<<endl;
	optprint(fout, "Version", CURRENT_VERSION);
	optprint(fout, "Music", options.music_switch);
	optprint(fout, "Sound", options.sound_switch);
	optprint(fout, "BlockTagShown", options.blocktag_switch);
	optprint(fout, "DebugInfoShown", options.show_debug_info);
	optprint(fout, "WeatherShown", options.show_weather);
	optprint(fout, "ItemTagShown", options.itemtag_switch);
	optprint(fout, "EntityTagShown", options.entitytag_switch);
	optprint(fout, "UseDurabilityBar", options.durability_bar);
	optprint(fout, "ViewSideEffect", options.view_side_fx);

	fout << "[GameRules]" << endl;
	optprint(fout, "PlayerMuttering", options.player_mutter);
	optprint(fout, "MobSpawning", options.mob_spawning);
	optprint(fout, "DeathDrop", options.death_drop);
	optprint(fout, "CreatureDrop", options.creature_drop);
	fout.close();
}
void NewOptions() {
	options.FirstRunInit();
	SaveOptions();
}
void ReadOptions() {
	if(!ExistFile(OPTIONS_PATH)) {
		DebugLog("没有发现配置文件，正在创建新文件...",DLT_WARN);
		firstRun = true;
		NewOptions();
	} else {
		firstRun = false;
	}
	DebugLog((string)"【读取】开始读取配置文件:"+OPTIONS_PATH);
	fstream fin(OPTIONS_PATH,ios::in);
	string tmp;
	fin>>tmp;	//Head
	optinput(fin, options.option_ver);
	optinput(fin, options.music_switch);
	optinput(fin, options.sound_switch);
	optinput(fin, options.blocktag_switch);
	optinput(fin, options.show_debug_info);
	optinput(fin, options.show_weather);
	optinput(fin, options.itemtag_switch);
	optinput(fin, options.entitytag_switch);
	optinput(fin, options.durability_bar);
	optinput(fin, options.view_side_fx);
	fin >> tmp;
	optinput(fin, options.player_mutter);
	optinput(fin, options.mob_spawning);
	optinput(fin, options.death_drop);
	optinput(fin, options.creature_drop);
	fin.close();
}

#define OPT_COL_COLOR WHITE
#define OPT_PRE_COL_COLOR EGERGB(74,231,54)
#define OPT_CUR_COL_COLOR RED
void DrawOptions() {
	const int ui_opt_col_left = uix * 0.11;
	const int ui_opt_col_top = uiy * 0.28;
	const int ui_opt_col_fs = 50 * (uiy / 1080.0);
	const int ui_opt_col_gap = 50 * (uiy / 1080.0);
	const int ui_opt_content_left = ui_opt_col_left+ui_opt_col_fs*2+250*(uix / 1920.0);
	const int ui_opt_content_right = uix - 100 * (uix / 1920.0);//ui_opt_content_left + uix * 0.5;
	const int ui_opt_content_top = uiy * 0.15f;
	const int ui_opt_esc_left = 60 * (uix / 1920.0);
	const int ui_opt_esc_top = uiy * 0.78;

	cls();

	setfont(55*(uiy/1080.0),0,"Determination Mono");
	if(uid ui_opt_col_pre_id == -2)
		setcolor(LIGHTRED);
	else
		setwcolor(CORNSILK);
	uid rt_opt_esc = pxprint(ui_opt_esc_left,ui_opt_esc_top,"[Esc:保存并返回]", 1);

	setfont(uid ui_subtitle_fs,0,"Determination Mono");
	setwcolor(YELLOW);
	pxprint(ui_opt_col_left,ui_opt_col_top - uid ui_subtitle_fs - 55 * (uiy / 1080.0),"选项");

	setfont(ui_opt_col_fs,0,"Determination Mono");
	for(size_t i = 0; i < uid ui_opt_cols.size(); ++i) {
		if(i == uid ui_opt_col_id) {
			setcolor(OPT_CUR_COL_COLOR);
			pxprint(ui_opt_col_left-ui_opt_col_fs+2, ui_opt_col_top + (ui_opt_col_fs+ui_opt_col_gap)*i,"》"+uid ui_opt_cols.at(i));
		} else if(i == uid ui_opt_col_pre_id) {
			setcolor(OPT_PRE_COL_COLOR);
			pxprint(ui_opt_col_left-2*(ui_opt_col_fs-2), ui_opt_col_top + (ui_opt_col_fs+ui_opt_col_gap)*i,"·  "+uid ui_opt_cols.at(i), 2);
		} else {
			setcolor(OPT_COL_COLOR);
			pxprint(ui_opt_col_left,ui_opt_col_top + (ui_opt_col_fs+ui_opt_col_gap)*i,uid ui_opt_cols.at(i));
		}
		SetRect(&uid rt_opt_cols[i],
		        ui_opt_col_left,
		        ui_opt_col_top + (ui_opt_col_fs+ui_opt_col_gap)*i,
		        ui_opt_col_left+ui_opt_col_fs*uid ui_opt_cols.at(i).size()/2,
		        ui_opt_col_top + (ui_opt_col_fs+ui_opt_col_gap)*i+ui_opt_col_fs);
	}
	setfont(40*(uiy/1080.0),0,"System");
	setcolor(CYAN);
	for(short j = 0; j < (uiy * 0.7 / 40); ++j)
		xyprintf(ui_opt_content_left-100,uiy*0.15+40*j,"!");

	setfont(Clamp<int>(55*(uiy/1080.0), 48, 65),0,"Determination Mono");
	setwcolor(PINK);
	midpxprint("—————— "+uid ui_opt_cols.at(uid ui_opt_col_id)+" ——————",ui_opt_content_top+10,ui_opt_content_left,ui_opt_content_right);

	if(uid ui_opt_col_id == 0) {
		//存档
		setfont(Clamp<int>(45*(uiy/1080.0),38,60),0,"Determination Mono");
		if(uid ui_opt_col_pre_id == -3)
			setwcolor(PINK);
		else
			setcolor(LIGHTCYAN);
		uid rt_opt_save_opendir = midpxprint("[Space:打开存档文件夹]",ui_opt_content_top+200 * (uiy / 1080.0),ui_opt_content_left,ui_opt_content_right);

		if(uid ui_opt_col_pre_id == -4)
			setwcolor(PINK);
		else
			setcolor(LIGHTCYAN);
		uid rt_opt_save_delete = midpxprint("[Delete:删除存档]",ui_opt_content_top+400 * (uiy / 1080.0),ui_opt_content_left,ui_opt_content_right, uid ui_opt_col_pre_id == -4?9:0);
	} else if(uid ui_opt_col_id == 1) {
		//控制
		const int _op_top = ui_opt_content_top+90 * (uiy / 1080.0);
		const int ctrl_fs = Clamp<int>(38 * (uiy / 1080.0), 35, 55);
		const int addi = ctrl_fs + 4 * (uiy / 1080.0);
		setfont(ctrl_fs,0,"Determination Mono");
		setwcolor(ORANGE);
		midpxprint("★基本操作★",_op_top,ui_opt_content_left,ui_opt_content_right,1);
		setcolor(LIGHTGRAY);
		midpxprint("AWSD: 移动玩家        ↑←↓→1~0: 背包物品选择",_op_top+addi,ui_opt_content_left,ui_opt_content_right);
		midpxprint("J: 使用当前物品    Backspace: 丢弃一个物品",_op_top+addi*2,ui_opt_content_left,ui_opt_content_right);
		midpxprint("K: 操作面前方块        Shift+Backspace: 丢弃一组物品",_op_top+addi*3,ui_opt_content_left,ui_opt_content_right);
		midpxprint("Ctrl:开启/关闭方块提示 Delete: 删除一个物品",_op_top+addi*4,ui_opt_content_left,ui_opt_content_right);
		midpxprint("Esc: 游戏主菜单   Shift+Delete: 删除一组物品",_op_top+addi*5,ui_opt_content_left,ui_opt_content_right);
		midpxprint("上述大部分操作可以用鼠标代替:",_op_top+addi*6,ui_opt_content_left,ui_opt_content_right,1);
		midpxprint("方块上左键：镐/斧挖掘方块/砍树   物品上左键：选择物品",_op_top+addi*7,ui_opt_content_left,ui_opt_content_right);
		midpxprint("方块上右键：操作方块   物品上右键：取物品",_op_top+addi*8,ui_opt_content_left,ui_opt_content_right);
		setwcolor(ORANGE);
		midpxprint("★合成操作★",_op_top+addi*9,ui_opt_content_left,ui_opt_content_right,1);
		setcolor(LIGHTGRAY);
		midpxprint("< > 以及鼠标左键:选择合成项目",_op_top+addi*10,ui_opt_content_left,ui_opt_content_right);
		midpxprint("Enter或鼠标右键:合成一次所选项目",_op_top+addi*11,ui_opt_content_left,ui_opt_content_right);
	}
	else if (uid ui_opt_col_id == 2)
	{	//规则
		static int _col_width = (ui_opt_content_right - ui_opt_content_left) / 2;
		static int _left_col_center = (ui_opt_content_right + ui_opt_content_left) / 2
			- _col_width / 2;
		static int _rset_top = ui_opt_content_top + 100 * (uiy / 1080.0);
		static int _rset_fs = Clamp<int>(45 * (uix / 1920.0), 36, 60);
		static int _rset_addi = static_cast<int>(_rset_fs * 1.2f);
		for (size_t _ = 0; _ < OPTION_RULE_ITEM_CNT; ++_)
		{
			setfont(_rset_fs, 0, "Determination Mono");
			if (uid ui_opt_rules_pre_id == _)
				setwcolor(PINK);
			else
				setcolor(LIGHTCYAN);
			uid rt_opt_rules.at(_) = midpxprint(uid ui_opt_rules_texts.at(_) + "      ", _rset_top + _ / 2 * _rset_addi,
				_ % 2 ? (ui_opt_content_right + ui_opt_content_left) / 2 : ui_opt_content_left,
				_ % 2 ? ui_opt_content_right : (ui_opt_content_right + ui_opt_content_left) / 2);

			color_t _swcolor = LIGHTGRAY;
			string _swtext = GetSwitch(*(uid ui_opt_rules_switches.at(_)), _swcolor, DS_RED_GREEN);
			setcolor(_swcolor);
			midpxprint(string((uid ui_opt_rules_texts.at(_)).length(), ' ') + _swtext, _rset_top + _ / 2 * _rset_addi,
				_ % 2 ? (ui_opt_content_right + ui_opt_content_left) / 2 : ui_opt_content_left,
				_ % 2 ? ui_opt_content_right : (ui_opt_content_right + ui_opt_content_left) / 2);
		}
	}
	else if (uid ui_opt_col_id == 3)
	{	//视频
		static int _col_width = (ui_opt_content_right - ui_opt_content_left) / 2;
		static int _left_col_center = (ui_opt_content_right + ui_opt_content_left) / 2
					- _col_width / 2;
		static int _vset_top = ui_opt_content_top + 100 * (uiy / 1080.0);
		static int _vset_fs = Clamp<int>(45 * (uix / 1920.0), 36, 60);
		static int _vset_addi = static_cast<int>( _vset_fs * 1.2f);
		for (size_t _ = 0; _ < OPTION_VIDEO_ITEM_CNT; ++_)
		{
			setfont(_vset_fs, 0, "Determination Mono");
			if (uid ui_opt_videos_pre_id == _)
				setwcolor(PINK);
			else
				setcolor(LIGHTCYAN);
			uid rt_opt_videos.at(_) = midpxprint(uid ui_opt_videos_texts.at(_) + "      ", _vset_top + _ / 2 * _vset_addi,
				_ % 2 ? (ui_opt_content_right + ui_opt_content_left) / 2 : ui_opt_content_left,
				_ % 2 ? ui_opt_content_right : (ui_opt_content_right + ui_opt_content_left) / 2);

			color_t _swcolor = LIGHTGRAY;
			string _swtext = GetSwitch(*(uid ui_opt_videos_switches.at(_)), _swcolor, DS_RED_GREEN);
			setcolor(_swcolor);
			midpxprint(string((uid ui_opt_videos_texts.at(_)).length(),' ') + _swtext, _vset_top + _ / 2 * _vset_addi,
				_ % 2 ? (ui_opt_content_right + ui_opt_content_left) / 2 : ui_opt_content_left,
				_ % 2 ? ui_opt_content_right : (ui_opt_content_right + ui_opt_content_left) / 2);
		}
	}
	else if(uid ui_opt_col_id == 4) {
		//音频
		setfont(Clamp<int>(45 * (uiy / 1080.0),38,60),0,"Determination Mono");
		if(uid ui_opt_col_pre_id == -5)
			setwcolor(PINK);
		else
			setcolor(LIGHTGREEN);
		uid rt_opt_mus_switch = midpxprint("音乐      ",ui_opt_content_top + 220 * (uiy / 1080.0),ui_opt_content_left,ui_opt_content_right, options.music_switch ? 9 : 0);
		if (uid ui_opt_col_pre_id == -6)
			setwcolor(PINK);
		else
			setcolor(LIGHTGREEN);
		uid rt_opt_snd_switch = midpxprint("音效      ", ui_opt_content_top + 290 * (uiy / 1080.0), ui_opt_content_left, ui_opt_content_right, options.sound_switch ? 9 : 0);
		string s="     ";
		color_t clr=WHITE;
		s += GetSwitch(options.music_switch,clr,DS_GRAY_GREEN);
		setcolor(clr);
		midpxprint(s,ui_opt_content_top+220 * (uiy / 1080.0),ui_opt_content_left,ui_opt_content_right);
		s = "     ";
		clr = WHITE;
		s += GetSwitch(options.sound_switch, clr, DS_GRAY_GREEN);
		setcolor(clr);
		midpxprint(s, ui_opt_content_top + 290 * (uiy / 1080.0), ui_opt_content_left, ui_opt_content_right);
	}
	const int _ver_fs = 50 * (uiy / 1080.0);
	setwcolor(LIGHTGRAY);
	setfont(_ver_fs,0,"Determination Mono");
	xyprintf(10,uiy - _ver_fs,("V"+strxhead(CURRENT_VERSION)).c_str());

	delay_fps(60);
}
void ShowSaveInExplorer() {
	DebugLog("打开存档文件夹...");
	stringstream ss;
	ss<<"explorer.exe"<<" \""<<LOCAL_SAVE_DIR<<"\"";

	WinExec(ss.str().c_str(),SW_SHOW);
	delay(80);
}
void DeleteSave() {
	int ch = MessageBox(hwnd,"你真的要删除存档吗？\n删除的存档将无法找回！","DarkVoxel2 DeleteSave",MB_ICONEXCLAMATION|MB_YESNO|MB_DEFBUTTON2);
	if(ch == IDNO || ch == IDCLOSE) {
		return;
	} else {
		DebugLog("删除存档...");
		stringstream ss;
		ss<<"cmd /c del "<<LOCAL_SAVE_DIR<<"*.dat /q & del "<<LOCAL_SAVE_DIR<<"Regions\\*.bmp";
		WinExec(ss.str().c_str(),SW_HIDE);
		SND("block\\anvil1");
	}
}
#define OPTION_DRAW_CD 60
void EnterOptions() {
	clock_t lastDraw=clock();
	clock_t lastKey=clock();
	uid ui_opt_col_id=0;
	uid ui_opt_col_pre_id = -1;
	while(is_run()) {
		if(clock() - lastDraw >= OPTION_DRAW_CD) {
			DrawOptions();
			lastDraw = clock();
		}
		if(kbhit() && (clock() - lastKey >= KEY_CD)) {
			if((K(VK_UP) || K('W'))
			        && uid ui_opt_col_id >= 0) {
				uid ui_opt_col_id--;
				SND("click");
				ClampA<short>(uid ui_opt_col_id,0,uid ui_opt_cols.size()-1);
			} else if((K(VK_DOWN) || K('S'))
			          && uid ui_opt_col_id < uid ui_opt_cols.size()-1) {
				uid ui_opt_col_id++;
				SND("click");
				ClampA<short>(uid ui_opt_col_id,0,uid ui_opt_cols.size()-1);
			} else if((K(VK_TAB) && K(VK_CONTROL))) {
				//Ctrl+Tab
				SND("click");
				uid ui_opt_col_id = (uid ui_opt_col_id + 1) % uid ui_opt_cols.size();
			} else if(K(VK_ESCAPE)) {
				//Esc
				SND("click");
				SaveOptions();
				while (K(VK_ESCAPE));
				break;
			}

			if(uid ui_opt_col_id == 0) {
				if(K(VK_SPACE)) {
					SND("click");
					ShowSaveInExplorer();
				} else if(K(VK_DELETE)) {
					SND("click");
					DeleteSave();
				}
			} else if(uid ui_opt_col_id == 1) {
				//ctrl
			}
			else if (uid ui_opt_col_id == 2)
			{	//rules

			}
			else if (uid ui_opt_col_id == 3)
			{	//video settings

			}
			else if(uid ui_opt_col_id == 4) {
				if(K('M')) {
					SND("click");
					options.music_switch = !options.music_switch;
					delay(50);
				}
				else if (K('S'))
				{
					SND("click");
					options.sound_switch = !options.sound_switch;
					delay(50);
				}
			}
			lastKey = clock();
		}
		if(mousemsg()) {
			mouse_msg msg = getmouse();
			uid ui_opt_col_pre_id = -1;	//defvalue
			for(size_t i = 0; i < uid rt_opt_cols.size(); ++i) {
				//鼠标选择左侧选项卡
				if(InRect(msg.x,msg.y,uid rt_opt_cols[i])) {
					if (msg.is_left() && msg.is_up()) //选择
					{
						SND("click");
						uid ui_opt_col_id = i;
					}
					else	//提前
						uid ui_opt_col_pre_id = i;
				}
			}
			if(InRect(msg.x,msg.y,uid rt_opt_esc)) {
				//保存并返回
				if(msg.is_left() && msg.is_up()) {
					SND("click");
					SaveOptions();
					break;
				} else
					uid ui_opt_col_pre_id = -2;
			} else if(uid ui_opt_col_id == 0) {
				//Save
				if(InRect(msg.x,msg.y,uid rt_opt_save_opendir)) {
					if (msg.is_left() && msg.is_up())
					{
						SND("click");
						ShowSaveInExplorer();
					}
					else
						uid ui_opt_col_pre_id = -3;
				} else if(InRect(msg.x,msg.y,uid rt_opt_save_delete)) {
					if (msg.is_left() && msg.is_up())
					{
						SND("click");
						DeleteSave();
					}
					else
						uid ui_opt_col_pre_id = -4;
				}
			}
			else if (uid ui_opt_col_id == 2)
			{	//Rules
				uid ui_opt_rules_pre_id = -1;	//defval
				for (size_t _ = 0; _ < OPTION_RULE_ITEM_CNT; ++_)
				{
					if (InRect(msg.x, msg.y, uid rt_opt_rules.at(_)))
					{
						if (msg.is_left() && msg.is_up())
						{
							SND("click");
							*(uid ui_opt_rules_switches.at(_)) = !*(uid ui_opt_rules_switches.at(_));
						}
						else
							uid ui_opt_rules_pre_id = _;
					}
				}
			}
			else if (uid ui_opt_col_id == 3)
			{	//Video
				uid ui_opt_videos_pre_id = -1;	//defval
				for (size_t _ = 0; _ < OPTION_VIDEO_ITEM_CNT; ++_)
				{
					if (InRect(msg.x, msg.y, uid rt_opt_videos.at(_)))
					{
						if (msg.is_left() && msg.is_up())
						{
							SND("click");
							//DebugLog(ToString(_) + " is pressed");
							*(uid ui_opt_videos_switches.at(_)) = ! *(uid ui_opt_videos_switches.at(_));
						}
						else
							uid ui_opt_videos_pre_id = _;
					}
				}
			}
			else if(uid ui_opt_col_id == 4) {
				//Music
				if(InRect(msg.x,msg.y,uid rt_opt_mus_switch)) {
					if (msg.is_left() && msg.is_up())
					{
						SND("click");
						options.music_switch = !options.music_switch;
						if (!options.music_switch)
						{
							STOPMUS();
						}
						else {
							if(!g_playing)
								MUSLOOP("music\\theme2");
						}
					}
					else
						uid ui_opt_col_pre_id = -5;
				}else if (InRect(msg.x, msg.y, uid rt_opt_snd_switch)) {
					if (msg.is_left() && msg.is_up())
					{
						SND("click");
						options.sound_switch = !options.sound_switch;
					}
					else
						uid ui_opt_col_pre_id = -6;
				}
			}
		}
	}
}

#define DMUI_BTN_PRE_COLOR PINK
#define DMUI_BTN_COLOR LIGHTCYAN
#define DMUI_BTN_COLOR_ADAPT(n) if(uid ui_main_pre_id == n)\
		setcolor(DMUI_BTN_PRE_COLOR);\
	else\
		setcolor(DMUI_BTN_COLOR);
void DrawMainUI() {
	cls();
	setfont(180*(uiy / 1080.0),0,"Courier New", 30 * sin(clock() / 1000.0), 0,0,0,0,0);
	setcolor(YELLOW);
	xyprintf(100*(uix/1920.0),uiy*0.3,"DarkVoxel");
	setfont(200*(uiy / 1080.0)*(1.0+0.3*sin(clock()/1000.0)),0,"System");
	setcolor(LIGHTGREEN);
	xyprintf((9* 180 * (uix / 1920.0) / 2 - 50 * (uix / 1920.0)), uiy*0.3+235*(uiy/1080.0)+20*sin(clock()/1000.0), "2");
	//xyprintf((9 * 180 / 2 - 50), uiy*0.3 + 95 * (uiy / 1080.0), "2");

	const int _ver_left = uix*0.05f;
	const int _ver_fs = 50;
	const int _ver_top = uiy - _ver_fs*2-200*(uiy / 1080.0);

	setcolor(LIGHTGRAY);
	setfont(_ver_fs,0,"Determination Mono");
	xyprintf(_ver_left,_ver_top,("V"+strxhead(CURRENT_VERSION)).c_str());
	pxprint_format(_ver_left,_ver_top+_ver_fs,
		"[{text:\"由 \",color:#light_gray}," 
	     "{text:\"Wormwaker\",color:#purple,waveColor:1s},"
	     "{text:\" 倾情打造\",color:#light_gray}]");

	const int _right_dist = uix * 0.35f;
	const int _top = uiy * 0.35f;
	const int _gap = 200*(uiy / 1080.0);
	const int _btn_left = uix - _right_dist;
	const int _btn_right = uix;
	setfont(80*(uiy/1080.0),0,"Determination Mono");
	DMUI_BTN_COLOR_ADAPT(0)
	uid rt_splay = midpxprint("[Space:单人]", _top, _btn_left, _btn_right, 1);
	DMUI_BTN_COLOR_ADAPT(1)
	uid rt_opt = midpxprint("[O:选项]", _top + _gap, _btn_left, _btn_right, 1);
	DMUI_BTN_COLOR_ADAPT(2)
	uid rt_leave = midpxprint("[Esc:离去]", _top + _gap * 2, _btn_left, _btn_right, 1);

	//pxprint_format(0, uiy / 2, "[{text:\"pxprint_format&\",color:#light_cyan,size:40s},{text:\"函数\",color:#0;250;154,size:37s},{text:\"&测试\",color:#&13,size:48s},{text:\"!!\",color:#red,size:65s}]");
	delay_fps(60);
}
#define MUI_DRAW_CD 60
void EnterMainUI() {
	clock_t lastDraw=clock();
	MUSLOOP("music\\theme2");
	setbkmode(TRANSPARENT);
	while(is_run()) {
		if(clock() - lastDraw >= MUI_DRAW_CD) {
			DrawMainUI();
			lastDraw = clock();
		}
		if(mousemsg()) {
			mouse_msg msg = getmouse();
			int mx=msg.x,my=msg.y;
			if(InRect(mx,my,uid rt_splay)) {
				if(msg.is_left() && msg.is_up()) {
					SND("click");
					STOPMUS();
					SinglePlayer();
					MUSLOOP("music\\theme2");
				} else
					uid ui_main_pre_id = 0;
			} else if(InRect(mx,my,uid rt_opt)) {
				if(msg.is_left() && msg.is_up()) {
					SND("click");
					ReadOptions();
					EnterOptions();
				} else
					uid ui_main_pre_id = 1;
			} else if(InRect(mx,my,uid rt_leave)) {
				if(msg.is_left() && msg.is_up()) {
					SND("click");
					STOPMUS();
					imagefilter_blurring(NULL,0x00FF,0xFFFF);
					delay_fps(60);
					delay(400);
					Leave();
				} else
					uid ui_main_pre_id = 2;
			} else {
				uid ui_main_pre_id = -1;
			}
		} else if(kbmsg()) {
			if(K(VK_ESCAPE)) {
				SND("click");
				STOPMUS();
				Leave();
			} else if(K(VK_SPACE) || K(VK_RETURN)) {
				SND("click");
				STOPMUS();
				SinglePlayer();
				MUSLOOP("music\\theme2");
			} else if(K('O')) {
				SND("click");
				ReadOptions();
				EnterOptions();
			}
		}
	}
}
////////////////////////////////////////////////////////////////////////////////////////
int main() {
	CreateFile(LOG_NAME,"===========================================");
	DebugLog("        DarkVoxel2 运行日志   ");
	uix = GetSystemWorkAreaWidth();
	uiy = GetSystemWorkAreaHeight() - GetSystemMetrics(SM_CYCAPTION);
	initgraph(uix, uiy, INIT_RENDERMANUAL);
	delay_fps(60);
	setbkcolor(BLACK);
	setcaption(strcat_cc("DarkVoxel ", CURRENT_VERSION));
	hwnd = getHWnd();
	setrendermode(RENDER_MANUAL);
	ege_enable_aa(true);
	SetWindowPos(hwnd,NULL,0,0,0,0,SWP_NOSIZE|SWP_NOZORDER);	//左上贴着
	ShowWindow(hwnd, SW_MAXIMIZE);

	_GetTheDirs();//获取存档路径
	g_block_px = DEF_BLOCK_PX;
	OnBlockPXChanged();
	uid InitSizes();
	AddFontResource((LOCAL_DATA_DIR + "DeterminationMono.otf").c_str());
	Splash();
	PrepareSounds();
	if(firstRun) {
		Beginning();
	}
	EnterMainUI();

	Leave();
	return 0;
}
