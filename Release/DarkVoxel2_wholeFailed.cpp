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
//64位程序 
//#define CAREFUL_TRACK					//调试跟踪 
#define ITEM_REAL_SHAPE
#define NO_NVMAP_BLURRING
#define CURRENT_VERSION "v2.0.1.1"
//Extra Compiling Options
//-lgdi32 -luser32 -mwindows -lwininet -lws2_32 -lgraphics64 -luuid -lmsimg32 -limm32 -lole32 -loleaut32 -lgdiplus -lwinmm

#pragma warning(disable:4018,4102)
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <io.h>
#include <string>
#include <fstream> 
#include <sstream>
#include <stdexcept>
#include <cmath>
#include <algorithm>
#include <vector>
#include <array>
#include <stack>
#include <list>
#include <map>
#include "graphics.h"	//Use EGE 
#include "ege\sys_edit.h"
using namespace std;
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

COLORREF CJZAPI StepColor(COLORREF startColor, COLORREF endColor, double rate) {
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
	return EgeColor(StepColor(WinColor(startColor),WinColor(endColor),rate));
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
		if(isOn)	s+="■コ";
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
inline float CJZAPI Distance(float x1, float y1, float x2, float y2) {
	return sqrt((x1-x2)*(x1-x2)+(y1-y2)*(y1-y2));
}
inline float CJZAPI Distance(long x1, long y1, long x2, long y2) {
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
inline bool CJZAPI Percent(_T prob) {
	return (RandomRange(0,100,true,false)<prob);
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
HANDLE CJZAPI CreateThread(LPTHREAD_START_ROUTINE func_name) {
	// ###警告！！函数格式必须为 DWORD FUNC(LPVOID XXX) ###
	return ::CreateThread(NULL,0,func_name,NULL,0,NULL);
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
inline string CJZAPI unquote(const string& src) {
	return strxtail(strxhead(src));
}
inline void CJZAPI WarnLogTip(HWND hwnd, string strText,
                              string moduleName=""
                             ) {
	string db=moduleName+" WARNING: "+strText;
	db = strrpc((char*)db.c_str(),"\n"," | ");	//删除内部换行
	DebugLog(db,DLT_WARN);
	ErrorTip(hwnd,strText,"DarkVoxel2 "+moduleName+" WARNING");
}
inline void CJZAPI ErrorLogTip(HWND hwnd, string strText,
                               string moduleName=""
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
RECT CJZAPI midprint(string strText,int lineNum=NEXT_LINE_NUM,int _left=0, int _right=uix, DWORD dwInternalDelayMs=0) {
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
RECT CJZAPI midprinty(string strText,int y,int _left=0, int _right=uix, DWORD dwInternalDelayMs=0) {
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

vector<string> CJZAPI CutLine(string line,char sep=' ') {
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
#define CHN_MIN_X 12
#define CHNITEM_MAX 400
PIMAGE AttainChnImage(string chn, color_t clr) {
	XCPT_L
	for(short i = 0; i < chn_items.size(); ++i)
		if(chn_items.at(i).chn == chn && chn_items.at(i).clr == clr)
			return chn_items.at(i).img;
	//else create one
	PIMAGE img = newimage(CHN_MIN_X,CHN_MIN_X);
	setfont(CHN_MIN_X,0,"宋体",img);
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
	if(chn_items.size() > CHNITEM_MAX)
	{
		delimage(chn_items.at(0).img);
		chn_items.erase(begin(chn_items));
	}
	return img;
	XCPT_R
}

RECT pxprint(int x, int y, string s, DWORD dwInternalDelayMs=0) {
	XCPT_L
	color_t clr = getcolor();
	int sz = textheight('%');
	short add = 0;
	for(short i = 0; i < s.length() /*- s.length()%2*/;) {
//		string tmp = s.substr(i,(isprint(s.at(i)) ? 2 : 1));
		string tmp = s.substr(i,KnowItsLen(s.at(i)));
//		DebugLog(tmp);
		if(tmp.size() == 1) {
			setfont(sz,0,"FixedSys");
			setcolor(clr);
			xyprintf(x+add,y,"%s",tmp.c_str());
			add += sz/2;
			i += 1;
		} else {
			PIMAGE img = AttainChnImage(tmp, clr);
			_resize(img,sz,sz);	//重写后的可用的函数
			//		putimage_withalpha(NULL, img, x + sz * i, y);
			//		putimage_withalpha(NULL, img, 80,80);
//			putimage_alphatransparent(NULL,img, x+add, y, clr, 255);
//			putimage(x+add, y, img);
			putimage_alphatransparent(NULL, img, x+add, y, BLACK, 255);
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
#define _OFFSET_X 0
RECT CJZAPI midpxprinty(string strText,int y, int _left=0, int _right=uix, DWORD dwInternalDelayMs=0) {
	//自定义 y
	int fy=textheight('1');	//font height
	int stot_x = textwidth(strText.c_str());
	int x=_left+(_right-_left)/2-stot_x/2 + _OFFSET_X;
	pxprint(x,y,strText,dwInternalDelayMs);
	RECT rt;
	rt.left = x;
	rt.right = x+stot_x;
	rt.top = y;
	rt.bottom = y+fy;
	return rt;	//返回矩形
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

namespace ui_data {
	RECT rt_splay;
	RECT rt_opt;
	RECT rt_leave;
	short ui_main_pre_id = -1;

	const int ui_subtitle_fs = 65;

	RECT rt_opt_esc;
	RECT rt_opt_save_opendir;
	RECT rt_opt_save_delete;
	RECT rt_opt_mus_switch;
	const array<string,3> ui_opt_cols = {"存档","控制","音乐"};
	array<RECT,3> rt_opt_cols;
	short ui_opt_col_id=0;
	short ui_opt_col_pre_id = -1;

	RECT rt_play_create_new;
	RECT rt_play_giveup;
	short ui_play_cc_pre_id = -1;

	RECT rt_nvcfg_done;
	RECT rt_nvcfg_cancel;
	const int ui_nvcfg_title_fs = 65;
	const int ui_nvcfg_text_fs = 55;
	const int ui_nvcfg_btn_fs = 60;
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
}
#define uid ::ui_data::
	namespace nvcfg {
	//注：如果不用指针会导致进入主函数前的初始化，导致RE
	sys_edit* seed_box=nullptr;
	sys_edit* pname_box=nullptr;

	const int seed_box_w = 350;
	const int seed_box_h = uid ui_nvcfg_text_fs;
	const color_t seed_box_color = PINK;

	const int pname_box_w = 500;
	const int pname_box_h = uid ui_nvcfg_text_fs;
	const color_t pname_box_color = LIGHTGREEN;

	string tip;

#define SEEDBOX_INPUT_MAXLEN 16
#define PNAMEBOX_INPUT_MAXLEN 32
#define SEED_MAX (ULONG_MAX-1)

	void InitBoxes() {
		TrackLog("输入框初始化...");
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
		seed_box->size(seed_box_w, seed_box_h+4);
		seed_box->setmaxlen(SEEDBOX_INPUT_MAXLEN);
		seed_box->setbgcolor(GRAY_11);
		seed_box->setcolor(seed_box_color);
		seed_box->setfont(uid ui_nvcfg_text_fs, 0, "FixedSys");
		seed_box->settext(ToString(seed).c_str());	//set  seed
		seed_box->visable(false);

		pname_box->create(false);	//单行
		pname_box->move(uix/2 - (nvcfg::seed_box_w+100+uid ui_nvcfg_text_fs*5)/2 + 100+uid ui_nvcfg_text_fs*4,
		                text_top+uid ui_nvcfg_text_fs+30);
		pname_box->size(pname_box_w, pname_box_h+4);
		pname_box->setmaxlen(PNAMEBOX_INPUT_MAXLEN);
		pname_box->setbgcolor(GRAY_11);
		pname_box->setcolor(pname_box_color);
		pname_box->setfont(uid ui_nvcfg_text_fs, 0, "FixedSys");
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

#define OPTIONS_PATH (DV_DIR+"options.ini")
#define PLAYERDATA_PATH (LOCAL_SAVE_DIR+"PLAYER.DAT")
#define WORLDDATA_PATH (LOCAL_SAVE_DIR+"WORLD.DAT")
#define BLOCKDATA_PATH (LOCAL_DATA_DIR+"BlockData.csv")
#define ITEMDATA_PATH (LOCAL_DATA_DIR+"ItemData.csv")
#define BIOMEDATA_PATH (LOCAL_DATA_DIR+"BiomeData.csv")
#define ENTITYDATA_PATH (LOCAL_DATA_DIR+"EntityData.csv")
#define BDJSONDATA_PATH (LOCAL_DATA_DIR+"BlockDefaultJSONData.prn")
#define EDJSONDATA_PATH (LOCAL_DATA_DIR+"EntityDefaultJSONData.prn")
#define IDJSONDATA_PATH (LOCAL_DATA_DIR+"ItemDefaultJSONData.prn")
#define TIPS_PATH (LOCAL_DATA_DIR+"GameTips.prn")
#define RECIPE_PATH (LOCAL_DATA_DIR+"Recipes.prn")
#define ACHVDATA_PATH (LOCAL_DATA_DIR+"Achievements.csv")
#define CRAFTTYPEDATA_PATH (LOCAL_DATA_DIR+"CraftTypeData.prn")
///////////////////////////////////////////////////////////////////
class DarkVoxel {
public:
bool g_playing=false;
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

void DirOffsetPos(long &x, long &y, DIR face, const char* tag=nullptr) {
	//依据方向位移坐标
	if (face == UP)
		y--;
	else if (face == RIGHTUP)
		x++, y--;
	else if (face == RIGHT)
		x++;
	else if (face == RIGHTDOWN)
		x++, y++;
	else if (face == DOWN)
		y++;
	else if (face == LEFTDOWN)
		x--, y++;
	else if (face == LEFT)
		x--;
	else if (face == LEFTUP)
		x--, y--;
	else {
		return;
	}
}
typedef ULONG BLOCK_ID, BLOCK_CLASS_ID, ENTITY_ID;

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
			return "byte";*/
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
		bool SetList(const char* key, const vector<string>& _list) {
			if(!HaveKey(key) || GetValueType(key,"JSON::SetList") != "list")
				return false;
			TrackLog("设置列表");
			SetValueRaw(key, ::ToString(_list).c_str());
			return true;
		}
		string GetValueType(const char* key, const char* tag=nullptr) const {
			string s = GetValueRaw(key);
//			DebugLog("GetValueType tag="+::ToString(tag)+" keyraw="+s);
			if(s.empty())	return "";	//not found
			return ::DarkVoxel::GetValueType(s);
		}
		template <typename _T>
		bool GetSimpleValue(const char* key, _T* result)	const
		{//简单类型直接使用模板函数转换
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
		//	TrackLog(".");
			raw = DarkVoxel::UncoverValue(raw, type);
		//		DebugLog("GetSimpleValue raw="+raw);
			*result = Convert<string,_T>(raw);
			return true;
		}

		bool GetComplexValue(const char* key, void* result)	const
		{		//复杂类型单独处理
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
typedef float LIGHTNESS;	//自己发光亮度，小数 

class BlockData {
	public:
		BLOCK_ID id;
		string cn_name;
		string en_name;
		color_t txt_color;
		short dur;
		short dur_lvl;

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

color_t ParseColor(const string& s) {
	//解析颜色
	if(s.empty()) {
		ErrorLogTip(NULL,"解析颜色字符串遇到错误：空串","ParseColor");
		return WHITE;
	}
	if(s[0] == '&') {	//16色
		UINT uClr = Convert<string,UINT>(strxhead(s,1));
		return Color16ToRGB(uClr);
	}
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

BLOCK_CLASS_ID ParseBlockClass(const string& s) {
	if(sequ(s,"air"))
		return BCLASS_AIR;
	else if(sequ(s,"liquid"))
		return BCLASS_LIQUID;
	else if(sequ(s,"nature"))
		return BCLASS_NATURE;
	else if(sequ(s,"passable"))
		return BCLASS_PASSABLE;
	else if(sequ(s,"tree"))
		return BCLASS_TREE;
	else if(sequ(s,"fruitful"))
		return BCLASS_FRUITFUL;
	else if(sequ(s,"block"))
		return BCLASS_BLOCK;
	else if(sequ(s,"craft"))
		return BCLASS_CRAFT;
	else if(sequ(s,"container"))
		return BCLASS_CONTAINER;
	else if(sequ(s,"door"))
		return BCLASS_DOOR;
	else if(sequ(s,"pickup"))
		return BCLASS_PICKUP;
	else if(sequ(s,"gravestone"))
		return BCLASS_GRAVESTONE;
	else if(sequ(s,"burner"))
		return BCLASS_BURNER;
	else if(sequ(s,"blossom"))
		return BCLASS_BLOSSOM;
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

ITEM_TYPE ParseItemType(const string& s) {
	if(sequ(s,"null"))
		return ITT_NULL;
	else if(sequ(s,"material"))
		return ITT_MATERIAL;
	else if(sequ(s,"block"))
		return ITT_BLOCK;
	else if(sequ(s,"pickaxe"))
		return ITT_PICKAXE;
	else if(sequ(s,"axe"))
		return ITT_AXE;
	else if(sequ(s,"bow"))
		return ITT_BOW;
	else if(sequ(s,"close_weapon"))
		return ITT_CLOSE_WEAPON;
	else if(sequ(s,"crossbow"))
		return ITT_CROSSBOW;
	else if(sequ(s,"gun"))
		return ITT_GUN;
	else if(sequ(s,"food"))
		return ITT_FOOD;
	else if(sequ(s,"rpg"))
		return ITT_RPG;
	else if(sequ(s,"launcher"))
		return ITT_LAUNCHER;	//其他发射器
	else if(sequ(s,"arrow"))
		return ITT_ARROW;
	else if(sequ(s,"bullet"))
		return ITT_BULLET;
	else if(sequ(s,"rocket"))
		return ITT_ROCKET;
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
bool firstRun=false;
void Splash() {
	TrackLog("...");
	setcolor(WHITE);
	setfont(95,0,"FixedSys");
	midprinty("Wormwake",uiy*0.3f);
	setcolor(LIGHTGRAY);
	setfont(70,0,"FixedSys");
	midprinty("G A M E S ",uiy*0.3f+100);
	setfont(33,0,"Consolas");
	setcolor(YELLOW);
	midprinty("LOADING...",uiy*0.3f+180);
	delay_fps(40);
	setfont(29,0,"宋体");
	setcolor(LIGHTGRAY);
	setbkmode(OPAQUE);
	const short sz = uiy - 120;
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
	ReadAchievementData();
	midprinty("  读取成就数据AchievementData...  ",sz);
	delay_fps(30);
	ReadEntityData();
	midprinty("  读取合成类型数据CraftTypeData... ",sz);
	delay_fps(30);
	ReadCraftTypeData();
	midprinty("读取方块默认JSON数据BlockDefaultJSONData...",sz);
	delay_fps(30);
	ReadBlockDefaultJSONData();
	midprinty(" 读取物品默认JSON数据ItemDefaultJSONData... ",sz);
	delay_fps(30);
	ReadItemDefaultJSONData();
	midprinty("             读取配方Recipes...             ",sz);
	delay_fps(30);
	ReadRecipes();
	ReadTips();

	randomize();
}
void sgap(long ms) {
	clock_t start=clock();
	while(clock() - start < ms)
		if(kbmsg() || K(VK_LBUTTON))
			break;
}
void Beginning() {
	//开端剧情
	static const int beg_text_fs = 55;
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
	midpxprinty("旅游业从此迅速衰微..",_etop+beg_text_fs+40,0,uix,0);
	setfont(beg_text_fs,0,"等线");
	midprinty("但电子竞技,元宇宙等高端技术越来越发达...",_etop+beg_text_fs*2+40*2,0,uix,0);
	setcolor(WHITE);
	midprinty("自然界逐渐在熟视无睹中渐渐退化.",_etop+beg_text_fs*3+40*3,0,uix,250);
	sgap(3300);
	cls();
	setcolor(DARKGRAY);
	midpxprinty("旅游业迅速衰微.",_etop+beg_text_fs+40,0,uix,0);
	midpxprinty("然而电子竞技,元宇宙等高端科学技术越来越发达....",_etop+beg_text_fs*2+40*2,0,uix,0);
	setfont(beg_text_fs,0,"等线");
	midprinty("自然界逐渐在熟视无睹中渐渐退化..",_etop+beg_text_fs*3+40*3,0,uix,0);
	setcolor(WHITE);
	midprinty("情况逐渐失控，",_etop+beg_text_fs*4+40*4,0,uix,300);
	sgap(3300);
	cls();
	setcolor(DARKGRAY);
	midpxprinty("旅游业衰微.",_etop+beg_text_fs+40,0,uix,0);
	midpxprinty("然而电子竞技,元宇宙等高端科学技术越来越发达.....",_etop+beg_text_fs*2+40*2,0,uix,0);
	midpxprinty("自然界逐渐在熟视无睹中渐渐退化...",_etop+beg_text_fs*3+40*3,0,uix,0);
	midpxprinty("情况逐渐失控..",_etop+beg_text_fs*4+40*4,0,uix,0);
	setcolor(CORNSILK);
	setfont(beg_text_fs,0,"等线");
	midprinty("整个世界转变成了一个充满符号的抽象世界",_etop+beg_text_fs*5+40*5,0,uix,200);
	sgap(1500);
	cls();
	setcolor(DARKGRAY);
	midpxprinty("然而电子竞技,元宇宙等高端科学技术越来越发达.....",_etop+beg_text_fs*2+40*2,0,uix,0);
	midpxprinty("自然界逐渐在熟视无睹中渐渐退化...",_etop+beg_text_fs*3+40*3,0,uix,0);
	midpxprinty("情况逐渐失控...",_etop+beg_text_fs*4+40*4,0,uix,0);
	setcolor(YELLOW);
	midpxprinty("整个世界转变成了一个充满符号的抽象世界",_etop+beg_text_fs*5+40*5,0,uix,0);
	setcolor(WHITE);
	midprinty("是谓『暗黑维度』.",_etop+beg_text_fs*6+40*6,0,uix,500);
	setcolor(LIGHTGRAY);
	setfont(beg_text_fs,0,"Calibri");
	midprinty("   DarkVoxel   ",_etop+beg_text_fs*7+40*7+5,0,uix,50);
	sgap(1000);
	cls();
	setcolor(YELLOW);
	midpxprinty("整个世界转变成了一个充满符号的抽象世界",_etop+beg_text_fs*5+40*5,0,uix,0);
	setcolor(WHITE);
	midpxprinty("是谓『暗黑维度』.",_etop+beg_text_fs*6+40*6,0,uix,0);
	setcolor(LIGHTGRAY);
	midpxprinty("       DarkVoxel   ",_etop+beg_text_fs*7+40*7+5,0,uix,5);
	sgap(4000);

	setcolor(DARKGRAY);
	midpxprinty("[按任意键开始游戏]",uiy*0.88,0,uix,50);
	delay(100);
	getch();
	cls();
}

#define DRAW_CD 0

typedef USHORT AI_TYPE, ENTITY_TYPE;
//实体类型
#define ETT_NULL 0
#define ETT_ARROW 1
#define ETT_BULLET 2
#define ETT_ROCKET 3
#define ETT_OTHER_PROJECTILE 4
#define ETT_MONSTER 5
#define ETT_ITEM 6
inline bool IsProjectileET(ENTITY_TYPE ett)
{
	return (ett == ETT_ARROW || ett == ETT_BULLET || ett == ETT_ROCKET || ett == ETT_OTHER_PROJECTILE);
}
ENTITY_TYPE ParseEntityType(const string& src) {
	if(sequ(src,"item"))
		return ETT_ITEM;
	if(sequ(src,"null"))
		return ETT_NULL;
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

#define DEF_BLOCK_PX 45
int g_block_px=DEF_BLOCK_PX;

int scene_left,scene_right,scene_top,scene_bottom;
inline int GetPlayerDrawLeft(void) {
	return scene_left + (scene_right - scene_left)/2 - g_block_px/2;
}
inline int GetPlayerDrawTop(void) {
	return scene_top + (scene_bottom - scene_top)/2 - g_block_px/2;
}
typedef ULONG ITEM_ID;

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
//Entity Status
#define EST_ALIVE 0
#define EST_DEAD 1

#define ET_HURT_TIME 350
class Entity {	//实体
	public:
		ENTITY_ID id;
		long uuid;
		
		long x;
		long y;
		DIR dir;
		short hp;
		AI_TYPE ai;		//current ai

		JSON tag;
		short status;
		clock_t lastMove;
		clock_t lastAttack;
		clock_t lastHurt;

		Entity() {
			x = y = 0;
			dir = RIGHT;
			id = 0;
			uuid=-1;
			hp = 0;
			lastMove = clock();
			lastHurt = clock() - ET_HURT_TIME;
			status = EST_ALIVE;
		}
		inline ENTITY_TYPE GetType(void)	const {
			return entitydata.at(id).type;
		}
		inline bool IsAlive(void)	const {
			return (hp > 0 && status == EST_ALIVE);
		}
		inline bool TimeToMove(void)	const
		{
			return (clock() - lastMove >= GetMoveCD());
		}
		inline bool IsBeingHurt(void)	const
		{
			return (clock() - lastHurt < ET_HURT_TIME);
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
		void DropLoot()
		{
			XCPT_L
			if(!entitydata.at(id).data.HaveKey("DropLoot")
			|| status == EST_DEAD)	return;
			vector<string> loots;
			bool suc = entitydata.at(id).data.GetComplexValue("DropLoot",&loots);
			if(!suc)
			{
				ErrorLogTip(NULL,"无法获取实体掉落物列表 id="+ToString(id),"Entity::DropLoot");
				return;
			}
			if(loots.empty())	return;
			for(short j = 0; j < loots.size(); ++j)
			{
				string item = UncoverValue(loots.at(j),"string");
				if(item.empty())	continue;
				vector<string> cut = CutLine(item,'*');
				ITEM_ID iid = Convert<string, USHORT>(UncoverValue(cut[0],"short"));
				short cnt = 1;
				if(cut.size() > 1 and cut[1] != "")
				{
//					cnt = Convert<string,short>(UncoverValue(cut[1],"short"));
					cnt = RangeChoice(cut[1]) ;
				}
				float chance=1.0f;
				if(cut.size() > 2 and cut[2] != "")
				{
					chance = Convert<string,float>(UncoverValue(cut[2],"float"));
				}
				if(Percent(chance*100))
				{
					SpawnItem(x,y,iid,cnt,GetItemInitialTag(iid).c_str());
				}
			}
			XCPT_R
		} 
		clock_t GetMoveCD(void) const
		{
			short move_cd;
			if(ai == AI_LINE)
			{
				bool suc = tag.GetSimpleValue<short>("MoveCD",&move_cd);
				if(!suc)
				{
					ErrorLogTip(NULL,"无法获取直线运动的实体移动冷却 en_name="+entitydata.at(id).en_name,"Entity::GetMoveCD");
					return 1000;
				}
				return move_cd;
			}else if(ai == AI_CHASE || ai == AI_FLEE)
			{
				bool suc = entitydata.at(id).data.GetSimpleValue<short>("MoveCD",&move_cd);
				if(!suc)
				{
					ErrorLogTip(NULL,"无法获取直线运动的实体移动冷却 en_name="+entitydata.at(id).en_name,"Entity::GetMoveCD");
					return 1000;
				}
				return move_cd;
			}
			ErrorLogTip(NULL,"无效的AI类型="+ToString(ai),"Entity::GetMoveCD");
			return 1000;
		}
		short GetAttack(void) const
		{
			short at=0;
			if(ai == AI_LINE)
			{
				bool suc = tag.GetSimpleValue<short>("Attack",&at);
				if(!suc)
				{
					ErrorLogTip(NULL,"无法获取直线运动的实体的攻击伤害 en_name="+entitydata.at(id).en_name,"Entity::GetMoveCD");
					return 0;
				}
				return at;
			}else if(ai == AI_CHASE || ai == AI_FLEE || ai == AI_IDLE)
			{
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
		void Die(void)
		{
//			DebugLog("Die type="+ToString(GetType()));
			if(GetType() == ETT_MONSTER)
			{
//				DebugLog("here");
				DropLoot();
			}
			hp = 0;
			status = EST_DEAD;
		}
		void Hurt(short damage)
		{	//Entit y::Hurt
			hp -= damage;
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
		void Attack()
		{	//Entit y::Attack
			XCPT_L
			if(IsProjectileET(GetType()))
			{	//投掷物 
				long owner = GetOwnerUUID();
				short att = GetAttack();
				for(short i = 0; i < players.size(); ++i)	//Players check
				{
					if(players.at(i).x == x && players.at(i).y == y //same coordination
					&& players.at(i).uuid != owner)					//not owner
					{
						players.at(i).Hurt(att);
						Die();
						return;
					}
				}
				for(short e = 0; e < entities.size(); ++e)	//Entities check
				{
					if(
					entities.at(e).uuid != uuid	//not itself
					&& entities.at(e).GetType() == ETT_MONSTER		//monster type
					&& entities.at(e).x == x && entities.at(e).y == y	//same coordination
					&& entities.at(e).uuid != owner
					)	//not owner
					{
						entities.at(e).Hurt(att);
						Die();
						return;
					}
				}
			}else if(GetType() == ETT_MONSTER)
			{
				if(!TimeToAttack())
					return;
				Player* ptr=nullptr;
				
				if(CanCloseAttack())
				{
					if(ptr=FacingPlayer(x,y,dir))
					{
						short dmg = GetAttack();
						ptr->Hurt(dmg);
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
						DirOffsetPos(_x,_y,dir,"Entity::Attack");
						SpawnProjectile(projID,_x,_y,dir,uuid,dmg,move_cd,range);
						lastAttack=clock();
					}
				}
			}
			XCPT_R
		}
		void CheckDeath()
		{
			if(OuttaWorld(x,y) 
			|| IsProjectileET(GetType()) 
			    && (world.GetBlock(x,y,"Entity::CheckDeath").IsBarrier()
				||  GetRStep() <= 0)
			)
			{
				Die();
				return;
			}
		}
		void Move()
		{	//Entit y::Move
			XCPT_L
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
		void AdaptData() 		throw(out_of_range) {
			XCPT_L
			ai = entitydata.at(id).GetAIType();
			if(!IsProjectileET(GetType()) && GetType() != ETT_ITEM)
			{
				hp = GetMaxHp();
			}else{
				hp = 1;
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
		short GetIndex(void)	const {
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
		void PlayerInteraction()
		{
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
					if(players.at(i).x == x && players.at(i).y == y) {
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
			XCPT_R
		}
		void Draw()
		{
			//绘画
			XCPT_L
			if(id == 0 || status == EST_DEAD 
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
			if(GetType() == ETT_ITEM) {
				ITEM_ID iid=0;
				short cnt=1;
				bool suc = GetItemData(iid, cnt, nullptr);
				if(!suc || iid == 0 || cnt == 0)
					return;
		//		DebugLog("item id="+ToString(iid)+" cnt="+ToString(cnt));
				setcolor(LightnessBlockColor(itemdata.at(iid).color, world.GetBlock(x,y,"Entity::Draw").light));
		#ifndef ITEM_REAL_SHAPE
				setfont(g_block_px * ITEM_SHAPE_RATIO,0,"FixedSys");
				pxprint(scr_x+g_block_px/2-g_block_px * ITEM_SHAPE_RATIO/2,
				        scr_y+g_block_px/2-g_block_px * ITEM_SHAPE_RATIO/2,
				        itemdata.at(iid).shape);
		#else
				setfont(g_block_px * ITEM_SHAPE_RATIO,0,"楷体");
				xyprintf(scr_x+g_block_px/2-g_block_px * ITEM_SHAPE_RATIO/2,
				         scr_y+g_block_px/2-g_block_px * ITEM_SHAPE_RATIO/2,
				         itemdata.at(iid).shape.c_str());
		#endif
				if(cnt > 1) {
					setfont(g_block_px * ITEM_CNT_RATIO,0,"FixedSys");
					setcolor(WHITE);
					pxprint(scr_x+g_block_px/2+2,
					        scr_y+g_block_px/2+g_block_px * ITEM_SHAPE_RATIO/2-10,
					        ToString(cnt));
				}
			} else if(IsProjectileET(GetType()))
			{	//proj
				setfont(g_block_px,0,"FixedSys");
				size_t index=0;
				if(entitydata.at(id).shapes.size() == 8)
					index = (dir - 1);
				else if(entitydata.at(id).shapes.size() == 4)
					index = (dir / 2 - 1);
				string shape = entitydata.at(id).shapes.at(index);
				color_t color = entitydata.at(id).colors.at(index);
				setcolor(LightnessBlockColor(color, world.GetBlock(x,y,"Entity::Draw").light));
		//		setcolor(WHITE);
				pxprint(scr_x,scr_y,shape);
			} else if(GetType() == ETT_MONSTER)
			{
				setfont(g_block_px,0,"FixedSys");
				size_t index=0;
				string shape = entitydata.at(id).shapes.at(index);
				color_t color = entitydata.at(id).colors.at(index);
				if(IsBeingHurt())
					color = StepColor(color,LIGHTRED,0.6);
				setcolor(LightnessBlockColor(color, world.GetBlock(x,y,"Entity::Draw").light));
				pxprint(scr_x,scr_y,shape);
			}
			else {
				ErrorLogTip(NULL,"无效的实体类型="+ToString(GetType()),"Entity::Draw");
				setcolor(entitydata.at(id).colors.at(GetIndex()));
				pxprint(scr_x,scr_y,entitydata.at(id).shapes.at(GetIndex()));
			}
			XCPT_R
		}
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
	et.AdaptData();
	et.lastMove=clock();
	et.lastAttack=clock();
	if(etag != nullptr)
		et.tag = ParseJSON(etag,"SpawnEntity");
	else {
		et.tag = GetDefaultEntityJSON(id);
	}
	DebugLog("SpawnEntity: id="+ToString(id)+" x="+ToString(x)+" y="+ToString(y)+" uuid="+ToString(et.uuid)+" tag="+et.tag.ToString());

	entities.push_back(et);
	return entities.end();
	XCPT_R
}
void SpawnItem(long x, long y, const char* etag=nullptr)
{
	SpawnEntity(1,x,y,RIGHT,etag);
	entities.at(entities.size()-1).RandomBlockPos();
}
void SpawnItem(long x, long y, ITEM_ID id, short cnt, const char* itag=nullptr)
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
void SpawnProjectile(ENTITY_ID id, long x, long y, DIR dir, long owner_uuid, short tot_attack, short move_cd, short range)
{
	string tag = "{Owner:"+ToString(owner_uuid)+"L,Attack:"+ToString(tot_attack)+"s,MoveCD:"+ToString(move_cd)+"s,RStep:"+ToString(range)+"s}";
	SpawnEntity(id,x,y,dir,tag.c_str());
//	DebugLog(entities.at(entities.size()-1).tag.ToString());
}

#define MAP_X_MIN 800
#define MAP_X_MAX 1200
#define MAP_Y_MIN 800
#define MAP_Y_MAX 1200

enum BIOME_ID : USHORT {
    biome_void=0,
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
};

typedef short LIGHT_LVL;	//1~16 当前发光亮度等级 
#define NORMAL_LIGHT_LVL 10		//正常值 
color_t CJZAPI LightnessBlockColor(color_t orig, LIGHT_LVL lvl)
{
	static const float min_ratio = 0.02f;
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
		Item GetFruit(void) const
		{
			Item ret;
			ret.id = ret.cnt = 0;
			if(!data.HaveKey("Fruit"))	return ret;
			//e.g.  {Fruit:"159s*1s*{Enchanted:1s}"}
			string raw="";
			BOOL bret = data.GetSimpleValue<string>("Fruit",&raw);
			if(!bret)	return ret;
			string fruitstr = UncoverValue(raw,"string");
			if(fruitstr.empty())	return ret;
			vector<string> cut = CutLine(fruitstr,'*');
			ret.cnt=1;
			ret.id = Convert<string,short>(UncoverValue(cut[0],"short"));
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
		bool HaveFruit(void) const
		{
			if(!tag.HaveKey("Fruit"))	return false;
			string raw;
			BOOL bret = data.GetSimpleValue<string>("Fruit",&raw);
			if(!bret)	return false;
			string fruitstr = UncoverValue(raw,"string");
			if(fruitstr.empty())	return false;
			return true;
		}
		Item PickFruit(void)
		{
			Item item; item.id=item.cnt=0;
			if(!HaveFruit())	return item;
			Item fruit = GetFruit();
			tag.RemoveItem("Fruit");	//移除 
			return fruit;
		}
		bool Break(long x, long y)
		{
			XCPT_L
			bool isContainer = HaveClass(BCLASS_CONTAINER);
			BLOCK_ID bid=id;
			if(p.inspblock == this)
			{	//否则会出事 
				CloseContainer();
			}
			JSON otag = this->tag;
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
				if(HaveClass(BCLASS_FRUITFUL) && HaveFruit())
				{
					sdropItems.push_back(GetFruit().id);
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
				} else if(!HaveClass(BCLASS_FRUITFUL) || !HaveFruit()){
					hasCnt = false;
				} else{
					//if(HaveClass(BCLASS_FRUITFUL) && HaveFruit())
					//{
						sdropCounts.push_back(GetFruit().cnt);
						sdropTags.push_back(GetFruit().tag.ToString());
						hasCnt = true;
					//}
				}
				
		//					DebugLog("iid="+ToString(iid)+" cnt="+ToString(cnt));
			} else {
				sdropItems.push_back(ToString(BlockToItem(bid))+"s");
				sdropTags.push_back(otag.ToString());	//注意：是物品方块标签 
				if(HaveClass(BCLASS_FRUITFUL) && HaveFruit())
				{
					Item fruit = GetFruit();
					sdropItems.push_back(fruit.id);
					sdropCounts.push_back(fruit.cnt);
					sdropTags.push_back(fruit.tag.ToString());
					hasCnt = true;
				}else
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
				DebugLog(ToString(edrops));
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
				return Break(x,y);
			}
			XCPT_R
			return true;
		}
		Item& GetContainerItem(short index) const	//支持get, set 
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

class World {
	public:
		vector<Block> blocks;
		long map_x;
		long map_y;
		
		long time;	//世界时间 
		
		PIMAGE nearbyMap;
		vector<Item> insp_container;	//临时存放查看的容器内的物品 
		int insp_ctn_left, insp_ctn_top;	//屏幕坐标 
		int insp_w, insp_h;
#define insp_ctn_right (world.insp_ctn_left+world.insp_w*CONTAINER_SLOT_W)
#define insp_ctn_bottom (world.insp_ctn_top+world.insp_h*CONTAINER_ITEM_FS)
		short insp_precur;				//容器物品预索引 
		
		clock_t lastMinTick;
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
			map_x=map_y=0;
			if(!blocks.empty())
				Clear();
			g_block_px=DEF_BLOCK_PX;
			nearbyMap = nullptr;
			lastMinTick = clock();
			time=0;
			insp_precur=-1;
			insp_ctn_left=insp_ctn_top=0;
			insp_w=insp_h=0;
		}
		void UpdateInspectedContainer(const Block& b)
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
		void SaveInspectedContainer(Block& b)
		{	//保存容器物品至方块数据 
			XCPT_L
			vector<string> content;
			for(short j = 0; j < insp_container.size(); ++j)
			{
				if(insp_container.at(j).id != 0)
				{
					string s;
					s += "{Slot:";
					s += ToString(j);
					s += "s,Item:";
					s += ToString(insp_container.at(j).id);
					s += "s";
					if(insp_container.at(j).cnt > 1)
					{
						s += ",Count:";
						s += ToString(insp_container.at(j).cnt);
						s += "s";
					}
					if(!insp_container.at(j).tag.Empty())
					{
						s += ",tag:";
						s += insp_container.at(j).tag.ToString();
					}
					s += "}";
		//			DebugLog(s);
					content.push_back(s);
				}
			}
			TrackLog("1");
			if(b.tag.HaveKey("Content"))
				b.tag.SetList("Content",content);
			else{
				b.tag.AddItem("Content",ToString(content));
			}
		//	DebugLog("Save:"+b.tag.ToString());
			XCPT_R
		}
		short ContainerGainItem(ITEM_ID id, short cnt, const char* itag=nullptr)
		{
			XCPT_L
			//返回剩余个数 成功则返回0
			for(short i = 0; i < insp_container.size(); ++i) {
				if(insp_container.at(i).id == id
		//					&& (itag==nullptr || (!container.at(i).tag.Empty() && container.at(i).tag.ToString() == string(itag)))
				        && insp_container.at(i).cnt < insp_container.at(i).GetMaxHeap()) {
					short original_cnt = insp_container.at(i).cnt;
					short of_cnt=0;
					bool of = insp_container.at(i).Gain(cnt,&of_cnt);
					if(of) {
						cnt -= insp_container.at(i).GetMaxHeap() - original_cnt;
					}
					if(cnt <= 0) {
						cnt = 0;
						break;
					}
				} else if(insp_container.at(i).id == 0) {
					//空位
					insp_container.at(i).id = id;
					insp_container.at(i).cnt = cnt;
					if(itag)
						insp_container.at(i).tag = ParseJSON(itag,"Player::GainItem");
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
		inline void UpdateTime(void)
		{
			if(clock() - lastMinTick > 1000)
			{
				time++;
				lastMinTick = clock();
			}
			if(time >= 24 * 60)
				time=0;	//昼夜循环 
		}
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
		void ApproachPlayerLightSource(void)
		{	//手上的光源 !
			if(p.bag.HandItem().IsLightSource())
			{
				LIGHTNESS ln = p.bag.HandItem().GetOwnLightness();
				PointLight(ln,GetPlayerX(),GetPlayerY());
			}
		}
		void ApproachEntityLightSource(void)
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
/*#define PL_VIEW_W 16
#define PL_VIEW_H 15*/
#define PL_VIEW_W_HALF ((scene_right - scene_left - 50 ) / g_block_px / 2)
#define PL_VIEW_H_HALF ((scene_bottom - scene_top - 200 ) / g_block_px / 2)
#define PL_VIEW_TOP (GetPlayerY()-PL_VIEW_H_HALF)
#define PL_VIEW_BOTTOM (GetPlayerY()+PL_VIEW_H_HALF)
#define PL_VIEW_LEFT (GetPlayerX()-PL_VIEW_W_HALF)
#define PL_VIEW_RIGHT (GetPlayerX()+PL_VIEW_W_HALF)
#define SURROUNDING_RANGE GetPlayerX()-PL_VIEW_W_HALF*2,GetPlayerY()-PL_VIEW_H_HALF*2,GetPlayerX()+PL_VIEW_W_HALF*2,GetPlayerY()+PL_VIEW_H_HALF*2
		void UpdateSurroundingLight(void)
		{
			XCPT_L
			ShedArea(GetEnvironmentLight(),SURROUNDING_RANGE);	//基础全局亮度 
			ApproachBlocksLightness(SURROUNDING_RANGE);
			ApproachPlayerLightSource();
			ApproachEntityLightSource();
			XCPT_R
		}
		void Clear() {
			blocks.clear();
		}
		void PushBlock(Block& b) {
			XCPT_L
			blocks.push_back(b);
			XCPT_R
		}
		void DeleteNearbyMap(void)
		{
			if(!nearbyMap)	return;
			delimage(nearbyMap);
			nearbyMap=nullptr;
		}
		void UpdateNearbyMap(void)
		{
			DeleteNearbyMap();
			nearbyMap = PrintedMap(NBMAP_PX,GetPlayerX() - NBMAP_W_HALF,GetPlayerX() + NBMAP_W_HALF,
											GetPlayerY() - NBMAP_H_HALF, GetPlayerY() + NBMAP_H_HALF);
		}
		Block& GetBlock(long x, long y, const char* tag=nullptr) {
			XCPT_L

			if(x >= map_x) {
				ErrorLogTip(NULL,"方块X坐标越界："+ToString(x)+">="+ToString(map_x),"World::GetBlock"+(tag?ToString(tag):""));
				return blocks.at(0);
			} else if(y >= map_y) {
				ErrorLogTip(NULL,"方块Y坐标越界："+ToString(y)+">="+ToString(map_y),"World::GetBlock"+(tag?ToString(tag):""));
				return blocks.at(0);
			} else if(y * map_x + x >= blocks.size()) {
				ErrorLogTip(NULL,"方块坐标越界："+ToString(y * map_x + x)+">="+ToString(blocks.size())+"\nX="+ToString(x)+" Y="+ToString(y),"World::GetBlock"+(tag?ToString(tag):""));
				return blocks.at(0);
			}
			return blocks.at(y * map_x + x);
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
} world;

PIMAGE PrintedMap(int px=1, long _left = 0, long _right = world.map_x-1, long _top = 0, long _bottom = world.map_y-1)
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
void SaveMapToFile(const char* path)
{
	TrackLog("..");
	PIMAGE img_map = PrintedMap();
	saveimage(img_map, path);
	DebugLog("【保存】已将世界缩略地图保存至"+ToString(path));
}

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
		idt.data = ParseJSON(tmptmp,"ReadItemData");
//		idt.data.ToLog();

		itemdata.push_back(idt);
		++i;
	}
}
#define ITEM_SHAPE_RATIO 0.75
#define ITEM_CNT_RATIO 0.55
inline bool HaveDamageItemType(ITEM_TYPE type) {
	return (type == ITT_CLOSE_WEAPON || type == ITT_GUN || type == ITT_BOW
	        || type == ITT_CROSSBOW || type == ITT_RPG || type == ITT_LAUNCHER
	        || type == ITT_PICKAXE || type == ITT_AXE);
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
		void Init() {
			id = 0;
			cnt = 0;
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
		void AdaptDurability(void) {
			if(HaveDamageItemType(GetType())) {
				if(tag.HaveKey("Durability")) {
					tag.SetShort("Durability",itemdata.at(id).GetDurability());
				} else {
					tag.AddItem("Durability",ToString(itemdata.at(id).GetDurability())+"s");
				}
			}
		}
		bool Damage(short decr = 1) {
			//Item::耐久度减少
			if(HaveDamageItemType(GetType())) {
				short odur = GetDur();
				odur -= decr;
				if(odur <= 0) {	//爆掉了
					Clear();	//报废
				} else {			//耐久减少
					tag.SetShort("Durability",odur);
				}
				return true;
			} else {
				return false;
			}
		}
		void Clear() {
			id = 0;
			cnt = 0;
			tag.Clear();
		}
		inline ITEM_TYPE GetType(void)	const {
			return itemdata.at(id).type;
		}
		short GetDur(void)	const {
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
			if(HaveDamageItemType(GetType()) && GetDur() <= 0)
				Clear();	//报废
			if(cnt > GetMaxHeap())	//超最大堆叠 
				   cnt = GetMaxHeap();
            if(HaveDamageItemType(GetType()) && GetDur() > itemdata.at(id).GetDurability())	//超上限 
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
				ss<<"[攻击] "<<itemdata.at(id).GetAttackDamage()<<" \n";
				ss<<"[冷却] "<<itemdata.at(id).GetUseCD()<<"ms\n";
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
			if(HaveDamageItemType(type)) {
				ss<<"[耐久] "<<GetDur()<<" / "<<itemdata.at(id).GetDurability()<<endl;
			}

			if(type == ITT_BLOCK)
				ss<<"方块\n";
			else if(type == ITT_MATERIAL)
				ss<<"材料\n";
			else if(type == ITT_CLOSE_WEAPON)
				ss<<"近战武器\n";
			else if(type == ITT_AXE)
				ss<<"斧子类\n";
			else if(type == ITT_PICKAXE)
				ss<<"镐子类\n";
			else if(type == ITT_GUN)
				ss<<"消耗子弹\n";
			else if(type == ITT_BOW || type == ITT_CROSSBOW)
				ss<<"消耗箭矢\n";
			else if(type == ITT_FOOD)
				ss<<"食品\n";
			else if(type == ITT_RPG)
				ss<<"消耗火箭弹\n";
			else if(type == ITT_LAUNCHER)
				ss<<"其他发射器\n";
			else if(type == ITT_ARROW)
				ss<<"箭矢类\n";
			else if(type == ITT_BULLET)
				ss<<"枪弹类\n"; 
			else if(type == ITT_ROCKET)
				ss<<"火箭弹类\n"; 
			else if(type == ITT_OTHER_PROJECTILE)
				ss<<"其他投掷物\n"; 
			else
				ss<<"其他物品\n";
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
		void Draw(long x, long y, byte curMode, bool isSimple, color_t bgorig=EGERGB(25,25,25), bool small=false) {
			//Item::Dra w
			Normalize();
			int fs = small?CONTAINER_ITEM_FS:ITEM_FS;
			int sw = small?CONTAINER_SLOT_W:SLOT_W;
			setfont(fs,0,"FixedSys");
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
			pxprint(x,y,itemdata.at(id).shape);
			if(!isSimple) {
				setcolor(LIGHTGRAY);
				setfont((fs) * 0.6,0,"FixedSys");
				pxprint(x+(fs)*2,y," × ");
				setfont(fs,0,"FixedSys");
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

#define rui_left scene_right + 15
#define rui_right uix - 5
#define rui_text_top 20
#define inven_top 350
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
		byte GetCurMode(short index) const {
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

#define CSYS_DST_FS 44
#define CSYS_DST_CUR_FS 65
#define CSYS_SRC_FS 44
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
				bar(x-3,y-2,x+CSYS_DST_CUR_FS*1.6,y+CSYS_DST_CUR_FS);
				setfont(CSYS_DST_CUR_FS,0,"FixedSys");
			} else
				setfont(CSYS_DST_FS,0,"FixedSys");
			setcolor(itemdata.at(dst.id).color);
			pxprint(x,y,itemdata.at(dst.id).shape);
			if(dst.cnt > 1) {
				if(selected) {
					setfont(CSYS_DST_CUR_FS*ITEM_CNT_CRAFT_RATIO,0,"FixedSys");
					setwcolor(PINK);
					pxprint(x+CSYS_DST_CUR_FS+10,y+CSYS_DST_CUR_FS-CSYS_DST_CUR_FS*ITEM_CNT_CRAFT_RATIO+1,ToString(dst.cnt));
				} else {
					setfont(CSYS_DST_FS*ITEM_CNT_CRAFT_RATIO,0,"FixedSys");
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
					setfont(CSYS_SRC_FS,0,"FixedSys");
					setcolor(itemdata.at(src.at(i).id).color);
					pxprint(CSYS_LEFTBAR_W+(CSYS_SRC_FS+5)*i+10,y+2,itemdata.at(src.at(i).id).shape);
					if(src.at(i).cnt > 1) {
						setfont(CSYS_SRC_FS*ITEM_CNT_CRAFT_RATIO,0,"FixedSys");
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
#define CSYS_ITEM_TOP 100
#define CSYS_ITEM_CNT (csys_item_cnt_up*2+1)
#define CSYS_ITEM_H	(CSYS_DST_FS*1.7f)
#define CSYS_FIRST_CRAFT_CD 900
#define CSYS_FIRST_CRAFT_CD_MAX 2500
#define CSYS_CRAFT_CD 100
#define CSYS_CHOOSE_NEAR_RECIPE_CD 350
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

		static const int title_fs = 55;
		static const int craft_fs = 30;
		static const int tip_fs = 30;

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
		void RefreshPrecur(void)
		{
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
		void Draw(void) {
			//CraftSystem::
			setfillcolor(BLACK);
			bar(0,0,CSYS_LEFTBAR_W,uiy);
			setcolor(YELLOW);
			setlinestyle(PS_SOLID,0,2);
			line(CSYS_LEFTBAR_W,0,CSYS_LEFTBAR_W,uiy);

			setfont(title_fs,0,"FixedSys");
			setwcolor(YELLOW);
			pxprint(10,20,"合成");
			setfont(craft_fs,0,"FixedSys");
			setwcolor(WHITE);
			if(!active_recipes.empty())
				pxprint(10,80,ToString(active_recipes.size())+"个可用配方");
			else
				pxprint(10,100,"没有可用配方!");
			setwcolor(DARKGRAY);
			setfont(tip_fs,0,"FixedSys");
			pxprint(10,uiy-tip_fs*2,"< >:选择项目");
			pxprint(10,uiy-tip_fs,"Enter:合成");

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
		bool CraftOnce(RECIPE_ID rid, Inventory* bag)
		{
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
	bool PartialCheckIsOK() const
	{
		if(cond.type == ACDT_GET)
		{
			for(short i = 0; i < ITEM_CNT; ++i)
			{
				if(cond.GetThisOK(p.bag.items[i]))
				    return true;
			}
		}
		return false;
	}
};
vector<AchievementData> achievement_data;

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

#define PL_DEF_MAX_HP 50
#define PL_DEF_MAX_MP 10
#define PL_DEF_MAX_SP 50
#define PL_DEF_NAME "Player"
#define PL_DEF_SHAPE "界"
#define PL_DEF_COLOR YELLOW
#define PL_DEF_MOVE_CD 100
#define PL_DEF_REACH_RANGE 3.6
#define PL_HURT_TIME 400
#define RESPAWN_CD 7000
#define DISCARD_CD 300
#define OPERATE_CD 300
#define ACHV_SHOWN_TIME 4000

class Player {	//玩家类
	public:
		string name;	//玩家名
		long uuid;

		string shape;
		color_t color;

		long x;
		long y;
		DIR face;

		//血量
		short hp;
		short max_hp;
		//魔法值
		short mp;
		short max_mp;
		//饱腹值
		short sp;
		short max_sp;

		//背包
		Inventory bag;
		CraftSystem craft;

		//指针
		Block* preblock;	//鼠标指向的方块 
		Block* inspblock;	//查看的方块 
		COORD preblock_coord;
		COORD inspblock_coord;

		//时间戳
		clock_t lastMove;
		clock_t lastUse;
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
			shape=PL_DEF_SHAPE;
			color=PL_DEF_COLOR;
			x=y=0;
			uuid=0L;
			face = RIGHT;
			hp=max_hp=PL_DEF_MAX_HP;
			mp=max_mp=PL_DEF_MAX_MP;
			sp=max_sp=PL_DEF_MAX_SP;
			preblock = inspblock = nullptr;
			prevAchv = 0;
			lastMove=lastUse=lastHurt=lastSPHurt=lastDiscard=clock();
			lastHPRegen=lastOperate=lastMPRegen=lastSPDecrease=lastAchieve=clock();
			lastDeath=clock() - RESPAWN_CD;
			sp_step_cnter=0;
			dead=false;
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
		void Achieve(ACHV_ID aid)
		{
			MsgSndTip();
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
		void Hurt(short damage)
		{
			XCPT_L
			if(!TimeToHurt())
				return;		//处于无敌帧 
			
			hp -= damage;
			ClampA<short>(hp,0,max_hp);
			lastHurt = clock();
//			DebugLog("Hurt="+ToString(damage));
			
			XCPT_R
		}
		#define OVER_SP_RATIO 1.5f
		bool Eat(short food)
		{
			if(sp >= max_sp*OVER_SP_RATIO)
			{
				return false;
			}
			sp += food;
			ClampA<short>(sp,0,max_sp*OVER_SP_RATIO);
			return true;
		}
		void Birth(void)
		{
			x=world.map_x/2;	//AT THE
			y=world.map_y/2;	//CENTRE
			face = RIGHT;
			hp=max_hp=PL_DEF_MAX_HP;
			mp=max_mp=PL_DEF_MAX_MP;
			sp=max_sp=PL_DEF_MAX_SP;
			uuid = AttainNewUUID();
			prevAchv = 0;
			lastAchieve = clock() - ACHV_SHOWN_TIME;
			preblock = nullptr;
			lastMove=lastUse=lastHPRegen=lastMPRegen=lastSPDecrease=clock();
			lastDeath=clock() - RESPAWN_CD;
			sp_step_cnter=0;
			dead=false;
		}
		void FirstInit(string& pname) {
			//地图必须已经生成好了
			name=pname;
			shape=PL_DEF_SHAPE;
			color=PL_DEF_COLOR;
			
			Birth();

			bag.items[0].Reset(46,1);
			bag.items[1].Reset(47,1);
			bag.items[2].Reset(48,1);
//			bag.RandomInventory();
		}
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
			if(sp <= 0)
				return HS_DEATH;
			else if(sp <= max_sp * PL_VERY_HUNGRY_RATIO)
				return HS_VERY_HUNGRY;
			else if(sp <= max_sp * PL_HUNGRY_RATIO)
				return HS_HUNGRY;
			else if(sp <= max_sp * PL_FULL_RATIO)
				return HS_WELL;
			else if(sp <= max_sp * PL_OVERFULL_RATIO)
				return HS_FULL;
			return HS_OVERFULL;
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
				sp -= 1;
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
			return cd;
		}
		clock_t GetUseCD(void) const {
			if(opmode.spectator)
				return 0;
			return itemdata.at(bag.HandItemCRef().id).GetUseCD();
		}
		inline bool TimeToMove() const {
			return (clock() - lastMove >= GetMoveCD());
		}
		inline bool TimeToUse() const {
			return (clock() - lastUse >= GetUseCD());
		}
		inline bool TimeToHurt() const {
			return (clock() - lastHurt >= PL_HURT_TIME);
		}
		void OnPlayerDeath()
		{
			if(CurBlockCRef().id == 0)
			{	//如果是空的格子才放墓碑 
				world.SetBlock(x,y,RandomGravestone());
			}
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
			ClampA(x,0L,world.map_x-1);
			ClampA(y,0L,world.map_y-1);
		}
		void TryToMove(DIR dir) {
			if(!TimeToMove())
				return;
			sp_step_cnter++;
			DirOffsetPos(x,y,dir,"Player::TryToMove");
			this->face = dir;
			lastMove = clock();
		}
		short GainItem(ITEM_ID id, short cnt, const char* tag=nullptr) {
			//Player::
			//返回剩余个数 成功则返回0
			for(short i = 0; i < ITEM_CNT; ++i) {
				if(bag.items[i].id == id
//					&& (tag==nullptr || (!bag.items[i].tag.Empty() && bag.items[i].tag.ToString() == string(tag)))
				        && bag.items[i].cnt < bag.items[i].GetMaxHeap()) {
					short original_cnt = bag.items[i].cnt;
					short of_cnt=0;
					bool of = bag.items[i].Gain(cnt,&of_cnt);
					if(of) {
						cnt -= bag.items[i].GetMaxHeap() - original_cnt;
					}
					if(cnt <= 0) {
						cnt = 0;
						break;
					}
				} else if(bag.items[i].id == 0) {
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
//			DebugLog("cnt="+ToString(cnt));
			CheckAchvGets();
			return cnt;
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
};
vector<Player> players;
//<!>
#define p players.at(0)

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

void DrawBlock(Block& b, int _x, int _y) {
	XCPT_L
	if(b.id == 0 || !b.shape)	return;

	setfont(g_block_px,0,"FixedSys");
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
	pxprint(_x,_y,b.shape);

	XCPT_R
}
inline bool OuttaWorld(const long& _x, const long& _y) {
	if(_x < 0 || _y < 0
	        || _x >= world.map_x || _y >= world.map_y)
		return true;
	return false;
}
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
			Block& bref = world.GetBlock(abs_x,abs_y,"DrawSceneBlocks");
//            DebugLog(ToString(bref.id)+" "+ToString(abs_x)+","+ToString(abs_y));
			int scr_x=0;
			int scr_y=0;
			scr_x = GetPlayerDrawLeft() + (abs_x - GetPlayerX()) * g_block_px;
			scr_y = GetPlayerDrawTop() + (abs_y - GetPlayerY()) * g_block_px;
			DrawBlock(bref, scr_x,scr_y);
		}

	XCPT_R
}
void DrawDebugInfo() {
	static const int dbg_text_fs = 35;
	setfont(dbg_text_fs,0,"FixedSys");
	setcolor(LIGHTGRAY);
//	pxprint(10,10,"X="+ToString(GetPlayerX())+" Y="+ToString(GetPlayerY()));
//	pxprint(10,10,"L:"+ToString(-CSYS_ITEM_CNT/2+1)+" R:"+ToString(p.craft.active_recipes.size()-CSYS_ITEM_CNT/2-1));
	pxprint(10,60,"FPS="+ToString(getfps())+" topCur="+ToString(p.craft.top_cur));
	pxprint(10,10,"preTopCur="+ToString(uid craft_precur)+" Cur="+ToString(p.craft.Cur()));
	if(p.craft.Cur() < p.craft.active_recipes.size())
		pxprint(10,170,"curRecipe="+ToString(p.craft.active_recipes.at(p.craft.Cur())));
	pxprint(10,110,"p.json="+p.json.ToString());
	pxprint(10,160,"waitforRelease="+ToString(p.craft.waitForRelease));
}
void DrawPlayer() {
	//输出玩家
	setfont(g_block_px,0,"FixedSys");
	color_t color = GetPlayerColor();
	if(p.IsBeingHurt())
		color = LIGHTRED;
	setcolor(LightnessBlockColor(color, world.GetBlock(p.x,p.y,"DrawPlayer").light));
	midpxprinty(GetPlayerShape(),GetPlayerDrawTop(),scene_left,scene_right,0);
}
#define hotbar_left rui_left+ITEM_FS*2
#define hotbar_top inven_top+ITEM_FS
#define exten_w 200
#define inven_hotbar_left rui_left - exten_w
#define inven_hotbar_top hotbar_top
#define inven_hotbar_gap 20
void DrawInventory() {
	setfont(ITEM_FS,0,"FixedSys");
	setcolor(CYAN);
	midpxprinty("—————————",inven_top,
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
	static const int rui_text_fs = 45;
	setcolor(LIGHTGRAY);
	setlinestyle(PS_SOLID,0,3);
	line(rui_left-(p.bag.isOpen?exten_w:0),rui_text_top*0.7,rui_left-(p.bag.isOpen?exten_w:0),uiy-100);
	setfillcolor(BLACK);
	if(!p.bag.isOpen)
		bar(rui_left,0,uix,uiy);
	else
		bar(rui_left-exten_w,0,uix,uiy);

	setfont(rui_text_fs,0,"FixedSys");
	setwcolor(WHITE);
	midpxprinty(GetPlayerName(),rui_text_top,rui_left-(p.bag.isOpen?exten_w:0),rui_right-(p.bag.isOpen?exten_w:0));
	setwcolor(YELLOW);
	midpxprinty("X="+ToString(GetPlayerX())+" Y="+ToString(GetPlayerY()),rui_text_top+rui_text_fs,rui_left-(p.bag.isOpen?exten_w:0),rui_right-(p.bag.isOpen?exten_w:0));
	setwcolor(DARKGRAY);
	midpxprinty(world.GetTime(),uiy-rui_text_fs,0,rui_left-(p.bag.isOpen?exten_w:0));

	DrawPlayerBars(rui_left+50-(p.bag.isOpen?exten_w:0),rui_text_top+rui_text_fs*2);
	DrawInventory();
}
inline bool IsBarrier(Block& bk) {
	return bk.IsBarrier();
}

bool OuttaWorld(long x, long y, DIR dir) {
	DirOffsetPos(x,y,dir);
	return OuttaWorld(x,y);
}
bool IsBarrier(long ox, long oy, DIR dir) {
	if(OuttaWorld(ox,oy,dir))	return true;
	DirOffsetPos(ox,oy,dir);
	return IsBarrier(world.GetBlock(ox,oy,"IsBarrier"));
}
Player* FacingPlayer(long ox, long oy, DIR dir)
{
	if(OuttaWorld(ox,oy,dir))	return nullptr;
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
Entity* FacingEntity(long ox, long oy, DIR dir, bool onlyMonster=false)
{
	if(OuttaWorld(ox,oy,dir))	return nullptr;
	if(entities.empty())	return nullptr;
	DirOffsetPos(ox,oy,dir);
	for(short j = 0; j < entities.size(); ++j)
		if(entities.at(j).x == ox && entities.at(j).y == oy 
			&& (!onlyMonster || entities.at(j).GetType() == ETT_MONSTER))
		{
			return &entities.at(j);
		}
	return nullptr;
}

#define WHEEL_CD 30	//ms
bool ms_msg_trg=false;
clock_t lastWheel;
mouse_msg ms_msg;

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
					p.bag.lastSwap = clock();
				}else{
					if(p.bag.items[p.bag.precur].id != 0)
						p.bag.tmpItemFrom = p.bag.precur;
					p.bag.items[p.bag.precur].SwapWith(p.bag.tmpItem, p.bag.lastSwap,true);
				}
			} else if(K(VK_SHIFT) && p.bag.items[p.bag.precur].id != 0 )
			{
				short remain = world.ContainerGainItem(  p.bag.items[p.bag.precur].id,
											p.bag.items[p.bag.precur].cnt,
											p.bag.items[p.bag.precur].tag.ToString().c_str());
				p.bag.items[p.bag.precur].Lose(p.bag.items[p.bag.precur].cnt - remain);
				p.bag.lastSwap = clock();
			}
			
		}
	}else if(p.bag.precur != -1	
	        && K(VK_RBUTTON))
	{	//wanna take part of inventory item
		if(clock() - p.bag.lastSwap > SWAP_CD)
		{
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
			}else if(K(VK_SHIFT) && world.insp_container[world.insp_precur].id != 0 ){
				short remain = p.GainItem(  world.insp_container[world.insp_precur].id,
											world.insp_container[world.insp_precur].cnt,
											world.insp_container[world.insp_precur].tag.ToString().c_str());
				world.insp_container[world.insp_precur].Lose(world.insp_container[world.insp_precur].cnt - remain);
				p.bag.lastSwap = clock();
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
inline bool HaveCDItemType(ITEM_TYPE type) {
	return (type == ITT_PICKAXE || type == ITT_CLOSE_WEAPON || type == ITT_AXE
	        || type == ITT_GUN || type == ITT_BOW || type == ITT_CROSSBOW
	        || type == ITT_LAUNCHER || type == ITT_RPG);
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

	Block& bk = world.GetBlock(x,y,"KeyJ");

	if(itype == ITT_PICKAXE) {
		//镐是用来挖方块的 也可以砍树
		if(OuttaWorld(x,y) || !reach)
			return;
		short power=itemdata.at(id).GetPower();
		short power_lvl=itemdata.at(id).GetPowerLevel();
		bool ret = world.GetBlock(x,y,"KeyJ").Damage(power,power_lvl,x,y);
		if(!ret) {
//			ErrorLogTip(NULL,"镐子不够强，挖不动");
		}
		TrackLog("使用镐子");
		p.bag.HandItem().Damage(1);
	} else if(itype == ITT_AXE) {
		//斧子就是用来砍树类的 砍树效果比镐子好
		if(OuttaWorld(x,y) || !reach || !bk.HaveClass(BCLASS_TREE))
			return;
		short power=itemdata.at(id).GetPower();
		short power_lvl=itemdata.at(id).GetPowerLevel();
		bool ret = world.GetBlock(x,y,"KeyJ").Damage(power,power_lvl,x,y);
		if(!ret) {
//			ErrorLogTip(NULL,"斧子比较菜，砍不动");
		}
		TrackLog("使用斧子");
		p.bag.HandItem().Damage(1);
	} else if(itype == ITT_BLOCK) {
		//放置方块
		if(OuttaWorld(x,y) || bk.id != 0 || !reach 		//出界或已有方块
		|| (x==p.x&&y==p.y&&!blockdata.at(ItemToBlock(id)).HaveClass(BCLASS_PASSABLE)))	//或欲要在当前位置放不可通过的方块 
			return;
		world.SetBlock(x,y, ItemToBlock(id), biome_void, "KeyJ", p.bag.HandItemCRef().GetItemBlockTag().ToString().c_str());
		p.bag.HandItem().Lose(1);
		TrackLog("放置方块");
	} else if(itype == ITT_BOW || itype == ITT_GUN || itype == ITT_RPG || itype == ITT_LAUNCHER)
	{
		bool has=false;
		ITEM_ID proj = p.GetProjectile(itype,&has);
		if(!has)	return;	//no corresponding projectiles
		DIR face = GetDir8(x-p.x,y-p.y);
		long _x=p.x,_y=p.y;
		DirOffsetPos(_x,_y,face,"UseItem");
		if(world.GetBlock(_x,_y,"UseItem").IsBarrier())	return;	//against a block
		p.LoseItem(proj,1);
		SpawnProjectile(itemdata.at(proj).GetEntityID(), 
//		_x,_y, 
		p.x,p.y,
		face, p.uuid, 
		     entitydata.at(2).GetExtraDamage()+itemdata.at(p.bag.HandItemCRef().id).GetAttackDamage(),
			 itemdata.at(id).GetProjectileMovingCD(),itemdata.at(p.bag.HandItemCRef().id).GetRange());
	} else if(itype == ITT_FOOD)
	{
		short food = itemdata.at(id).GetFood();
		bool eaten = p.Eat(food);
		if(eaten)
		{
			(p.bag.tmpItem.id == 0 ? p.bag.HandItem() : p.bag.tmpItem).Lose(1);
			TrackLog("吃东西") ;
		}
	}
	else{
		return;
	}
	//item using success
	CheckAchvUses(p.bag.HandItemCRef());
	p.lastUse = clock();
	XCPT_R
}
void KeyJ() {
	long _x=p.x,_y=p.y;
	DirOffsetPos(_x,_y, p.face,"KeyJ");
	UseItem(_x,_y,true);
	if(!HaveCDItemType(p.bag.HandItemCRef().GetType()))
		while(K('J'));	//等待释放 
}
void CloseContainer()
{
	world.SaveInspectedContainer(*p.inspblock);
	world.insp_container.clear();	//释放内存 
	p.inspblock = nullptr;
}
void OperateBlock(long x, long y, bool reach) {
	//操纵方块
	XCPT_L
	if(!reach || clock() - p.lastOperate < OPERATE_CD)
		return;
	Block& bk = world.GetBlock(x,y,"KeyK");
	if(bk.HaveClass(BCLASS_DOOR)) {
		string openraw = bk.tag.GetValueRaw("Open");
		if(openraw.size() >= 2)
			bk.tag.SetShort("Open", 1-Convert<char,short>(openraw[0]));
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
			p.GainItem(fruits.id,fruits.cnt,fruits.tag.c_str());
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
	DirOffsetPos(x,y,p.face,"KeyK");
	if(OuttaWorld(x,y))
		return;
	OperateBlock(x,y,true);
}
struct OPTIONS {	//游戏配置
	bool music_switch;
	bool blocktag_switch;
	bool show_debug_info;
	string option_ver;

	OPTIONS() {
		Init();
	}
	void Init() {
		//每次运行时的初始化
		music_switch=false;
		blocktag_switch=false;
		show_debug_info=false;
		option_ver="";
	}
	void FirstRunInit(void) {
		//游戏第一次运行时的初始化
		music_switch=false;
		blocktag_switch=true;
		show_debug_info=false;
		option_ver = CURRENT_VERSION;
	}
} options;

void ClearGameMemories()
{	//清空进入游戏运行时的内存 
	world.Clear();
	entities.clear();
	players.clear();
	ITEM_FS = ITEM_DEF_FS;
}

void GameControl() {
	XCPT_L
	if(!g_playing)	return;

	if(p.IsAlive() && !uid bPaused) {
		if(K('A') || K(VK_LEFT)) {
			p.face = LEFT;
			if(opmode.spectator || !IsBarrier(p.x,p.y,p.face)) {	//outtaworld inside
				p.TryToMove(LEFT);
			}
		} else if(K('W') || K(VK_UP)) {
			p.face = UP;
			if(opmode.spectator || !IsBarrier(p.x,p.y,p.face)) {	//outtaworld inside
				p.TryToMove(UP);
			}
		} else if(K('S') || K(VK_DOWN)) {
			p.face = DOWN;
			if(opmode.spectator || !IsBarrier(p.x,p.y,p.face)) {	//outtaworld inside
				p.TryToMove(DOWN);
			}
		} else if(K('D') || K(VK_RIGHT)) {
			p.face = RIGHT;
			if(opmode.spectator || !IsBarrier(p.x,p.y,p.face)) {	//outtaworld inside
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
			            ||	(p.craft.firstCrafted == 1 && clock() - p.craft.lastFirstCraft >= CSYS_FIRST_CRAFT_CD))) {
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
				TrackLog("合成一次");
			}
		}
		if(p.IsAlive())
			for(char hi = '0'; hi < '0'+HOTBAR_ITEM_CNT; ++hi)
				if(K(hi))
					p.bag.cur = (Convert<char,short>(hi)+9)%10;
		if(K('J') && p.IsAlive()) {
			KeyJ();
		} else if(K('K') && p.IsAlive()) {
			KeyK();
		} else if(K(VK_BACK) && clock() - p.lastDiscard >= DISCARD_CD)
		{
			long _x=p.x, _y=p.y;
			DirOffsetPos(_x,_y,p.face,"GameControl");
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
			options.blocktag_switch = !options.blocktag_switch;
		}else if(K(VK_F3))
		{
			while(K(VK_F3));
			options.show_debug_info = !options.show_debug_info;
		}
#ifndef NOCHEATKEYS
		if(K(VK_F8) && p.IsAlive()) {
			MsgSndTip();
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
	if(K(VK_ESCAPE) 
	&& (uid bPaused || p.IsAlive())	//死亡时不能开菜单 
	)
	{
		while(K(VK_ESCAPE));
		uid bPaused = !uid bPaused;
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
		            ||	(p.craft.firstCrafted == 1 && clock() - p.craft.lastFirstCraft >= CSYS_FIRST_CRAFT_CD))) {
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
#define ITAG_FS (ITEM_FS*0.75)
#define ITAG_BOTTOM_GAP 30
void DrawItemTag(vector<string>& itemtag, int x, int y) {
	
	XCPT_L
	if(itemtag.empty())	return;
	size_t tag_w=1,tag_h=1;
	for(short i = 0; i < itemtag.size(); ++i)
		tag_w = max<size_t>(tag_w, itemtag.at(i).size()*ITAG_FS/2);
	tag_h = ITAG_FS*1.1f*itemtag.size();
	if(x+tag_w > uix)
		x-=(x+tag_w-uix);
	if(y+tag_h > uiy-ITAG_BOTTOM_GAP)
		y-=(y+tag_h-uiy+ITAG_BOTTOM_GAP);
	//以上是防出屏代码
	setfillcolor(BLACK);
	setlinestyle(PS_SOLID,0,2);
	setcolor(YELLOW);
	sidebar(x,y,x+tag_w+2,y+tag_h);
	setwcolor(WHITE);
	setfont(ITAG_FS,0,"FixedSys");
	for(short i = 0; i < itemtag.size(); ++i) {
		pxprint(x,y+ITAG_FS*1.1f*i,itemtag.at(i));
	}
	XCPT_R
}
inline color_t BlockTagColor(color_t src) {
//	return WaveColor(InvertedColor(src));
	return WHITE;
}
#define BTAG_FS 40			//字号 
#define BTAG_LIGHT_MIN 3	//亮度最低要求 

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
				color_t clr = itemdata.at(BlockToItem(bid)).color;
				string name = blockdata.at(bid).cn_name;
				string en_name = blockdata.at(bid).en_name;
				setfillcolor(BLACK);
				setcolor(LIGHTCYAN);
				setlinestyle(PS_SOLID,0,2);
				sidebar(ms_msg.x+2,ms_msg.y+2,
				        ms_msg.x+2+max(textwidth(name.c_str()),textwidth(en_name.c_str())),
				        ms_msg.y+2+BTAG_FS*2);
				setfont(BTAG_FS,0,"FixedSys");
				setwcolor(BlockTagColor(clr));
				pxprint(ms_msg.x+4,ms_msg.y+4,name);
				setwcolor(DARKGRAY);
				pxprint(ms_msg.x+4,ms_msg.y+4+BTAG_FS,en_name);
			}
		} else if(!options.blocktag_switch && p.preblock != nullptr && !uid block_op_tip.empty()) {
			//block operation tip drawing
			setfont(BTAG_FS,0,"FixedSys");
			setwcolor(WHITE);
			pxprint(ms_msg.x+4,ms_msg.y+4,uid block_op_tip);
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
	static const int title_fs = 75;
	XCPT_L
	if(!p.dead)	return;
	setfont(title_fs,0,"FixedSys");
	setwcolor(LIGHTRED);
	midpxprinty("重生中...",uiy*0.47,0,rui_left);
	setfont(45,0,"FixedSys");
	midpxprinty(ToString(RESPAWN_CD/1000 - int((clock() - p.lastDeath) / 1000)),uiy*0.47+70,0,rui_left);
	XCPT_R
}
void DrawAchieved()
{
	static const int achv_fs = 54;
	static const int desc_fs = 40;
	int rt_w = uix * 0.3;
	if(p.prevAchv != 0 && (clock() - p.lastAchieve <= ACHV_SHOWN_TIME))
	{
		string s, desc;
		desc = achievement_data.at(p.prevAchv).description;
		s += "『"+ achievement_data.at(p.prevAchv).cn_name + "』";
		setwcolor(WHITE);
		setlinestyle(PS_SOLID,0,2);
		setfillcolor(BLACK);
		sidebar((rui_left-(p.bag.isOpen?exten_w:0))/2-rt_w/2,uiy*0.72-achv_fs-40,
				  (rui_left-(p.bag.isOpen?exten_w:0))/2+rt_w/2,uiy*0.72+achv_fs*4);
		setwcolor(YELLOW);
		setfont(achv_fs,0,"FixedSys");
		midpxprinty("获得成就!",uiy*0.72-achv_fs,0,rui_left-(p.bag.isOpen?exten_w:0));
		setwcolor(CYAN);
		midpxprinty(s,uiy*0.72,0,rui_left-(p.bag.isOpen?exten_w:0));
		setwcolor(achievement_data.at(p.prevAchv).color);
		/*bar((rui_left-(p.bag.isOpen?exten_w:0))/2-achv_fs/2,uiy*0.72+achv_fs,
		    (rui_left-(p.bag.isOpen?exten_w:0))/2+achv_fs/2,uiy*0.72+achv_fs*2);*/
		midpxprinty(achievement_data.at(p.prevAchv).shape,uiy*0.72+achv_fs,0,rui_left-(p.bag.isOpen?exten_w:0));
		setfont(desc_fs,0,"FixedSys");
		setcolor(LIGHTGRAY);
		midpxprinty(desc,uiy*0.72+achv_fs*2,0,rui_left-(p.bag.isOpen?exten_w:0));
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
	static const int title_fs = 55;
	static const int btn_fs = 45;
	static const int ver_fs = 40;
	static const int gap = 40;
	static int _top = uiy * 0.2;
	static const int paused_intensity = 0xFF * 0.45f;
	imagefilter_blurring(NULL,0x01,paused_intensity);
	setfont(title_fs,0,"FixedSys");
	setwcolor(YELLOW);
	midpxprinty("游戏已暂停",_top);
	setfont(ver_fs,0,"FixedSys");
	setwcolor(DARKGRAY);
	midpxprinty(CURRENT_VERSION,uiy-ver_fs-30);
	setfont(btn_fs,0,"FixedSys");
	DPM_BTN_COLOR_ADAPT(0)
	uid rt_pm_back = midpxprinty("[Esc:回到游戏]",_top+title_fs+120);
	DPM_BTN_COLOR_ADAPT(1)
	uid rt_pm_achv = midpxprinty("[A:成  就]",_top+title_fs+120+btn_fs+gap);
	DPM_BTN_COLOR_ADAPT(2)
	uid rt_pm_save = midpxprinty("[S:保  存]",_top+title_fs+120+btn_fs*2+gap*2);
	DPM_BTN_COLOR_ADAPT(3)
	uid rt_pm_opt = midpxprinty("[O:选   项]",_top+title_fs+120+btn_fs*3+gap*3);
	DPM_BTN_COLOR_ADAPT(4)
	uid rt_pm_sq = midpxprinty("[Space:保存并退出]",_top+title_fs+120+btn_fs*4+gap*4);
	XCPT_R
}
void DrawInspection()
{
	XCPT_L
	if(p.inspblock == nullptr)	return;
	static const int insp_title_fs = 40;
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
	setfont(insp_title_fs,0,"FixedSys");
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
void DrawScene() {
	XCPT_L
	static const int death_intensity = 0xFF * 0.5f;

	cls();

	DrawSceneBlocks();
	DrawPlayer();
	DrawEntities();
	DrawInspection();
	if(p.dead)
		imagefilter_blurring(NULL,0xFF,death_intensity);
	//↑Real Scene 
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
		if(!entities.at(i).IsAlive()) {
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
void MajorChecks() {
	//主要更新
	XCPT_L
	CheckEntitiesUpdate();
	world.UpdateSurroundingLight();		//lightness
	XCPT_R
}
void CheckRespawn(void)
{
	if(!p.IsAlive() && clock() - p.lastDeath > RESPAWN_CD)
	{
		p.Birth();	//dead = false
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
	static const int title_fs = 60;
	static const int achv_fs = 45;
	static const int icon_fs = 50;
	static const int text_fs = 40;
	static const int info_fs = 35;
	static const int num_fs = 30;
	static const int achv_h = ACD_ACHV_H;
	static int top = ACD_TOP;
	static int rt_w = uix * 0.36f;
	static int below_top = ACD_BELOW_TOP;
	static int page_achv_cnt = ACD_PAGE_ACHV_CNT;
	static const color_t title_clr = YELLOW;
	static const color_t achv_clr = LIGHTGREEN;
	static const color_t def_clr = EGERGB(234,234,234);
	static const color_t desc_clr = EGERGB(122,122,122);
	static const color_t info_clr = GREEN;
	static const color_t num_clr = WHITE;
	cls();
	setfont(title_fs,0,"FixedSys");
	setcolor(title_clr);
	midpxprinty("成  就",top);
	setfont(info_fs,0,"FixedSys");
	setwcolor(info_clr);
	char buf[80];
	sprintf(buf,"%d/%d  %.1f%%",p.achievements.size(),achievement_data.size()-1,100*p.achievements.size()/float(achievement_data.size()-1));
	midpxprinty(buf, uiy - info_fs*1.8f);
	setcolor(DARKGRAY);
	setfont(35,0,"FixedSys");
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
		setfont(num_fs,0,"FixedSys");
		setcolor(unlocked?num_clr:LIGHTGRAY);
		pxprint(uix/2 - rt_w/2 + 30,below_top + achv_h * j, "#"+ToString(uid acd_top_cur+j));
		setfont(icon_fs,0,"FixedSys");
		setwcolor(unlocked?ad.color:EGERGB(12,12,12));
		pxprint(uix/2 - 90,below_top + achv_h * j, ad.shape);
		setfont(achv_fs,0,"FixedSys");
		setwcolor(unlocked?achv_clr:DARKGRAY);
		pxprint(uix/2 - 5,below_top + achv_h * j+2,ad.cn_name);
		setfont(text_fs,0,"FixedSys");
		setwcolor(unlocked?def_clr:RED);
		midpxprinty("【"+ad.definition+"】",below_top + achv_h * j+9+achv_fs);
		setcolor(unlocked?desc_clr:EGERGB(5,5,5));
		midpxprinty(ad.description,below_top + achv_h * j+9+achv_fs+text_fs);
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
	MsgSndTip();
}
void PausedMenuControl_Options(void)
{
	EnterOptions();
}
void Exit()
{
	setfont(55,0,"FixedSys");
	setcolor(YELLOW);
	midpxprinty("存档中请稍候",uiy - 150,0,uix,15);
	DebugLog("Esc退出游戏，正在存档...") ;
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
			uid bPaused = false;
		}else if(uid ui_pm_pre_id == 1)
		{
			PausedMenuControl_Achievements();
		}else if(uid ui_pm_pre_id == 2)
		{
			PausedMenuControl_Save();
		}else if(uid ui_pm_pre_id == 3)
		{
			PausedMenuControl_Options();
		}else if(uid ui_pm_pre_id == 4)
		{
			PausedMenuControl_Exit();
		}
	}
	if(kbhit())
	{
		if(K('A'))
		{
			while(K('A'));
			PausedMenuControl_Achievements();
		}else if(K('S'))
		{
			while(K('S'));
			PausedMenuControl_Save();
		}else if(K('O'))
		{
			while(K('O'));
			PausedMenuControl_Options();
		}else if(K(VK_SPACE))
		{
			while(K(VK_SPACE));
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
		p.inspblock = nullptr;
	}
	XCPT_R
}
#define MINOR_CHECK_CD 500
void MinorChecks() {
	//次要更新
	XCPT_L
	p.craft.LimitTopCur();
	p.UpdateCraftPlaces();
	p.UpdateCraftRecipes();
	world.UpdateTime();
	world.UpdateBurnerFuelValues(SURROUNDING_RANGE);
	p.CheckIndexesChanges();
	
	world.UpdateNearbyMap();
	CheckAchvGets();
	CheckRespawn();
	CheckInspection();
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
/*#define GCHECK_CD 5
DWORD ThrGameChecks(LPVOID none) {
	XCPT_L
	DebugLog("[ThrGameChecks] 开启游戏更新线程");
	clock_t lastMinorCheck=clock();
	while(is_run() && g_playing) 
	{
		XCPT_L
		api_sleep(GCHECK_CD);
		XCPT_R
	}
	DebugLog("[ThrMouseTips] 关闭游戏更新线程");
	XCPT_R
	return 0;
}*/
void InGame() {
	//游戏主体函数
	XCPT_L
	
	g_playing = true;
	uid bPaused = false;
	uid ui_pm_pre_id = -1;
	clock_t lastDraw=clock();
	clock_t lastMinorCheck=clock();
	lastWheel = clock();

	scene_left = 50;
	scene_right = uix * 0.7;
	scene_top = 20;
	scene_bottom = uiy - scene_top;
	AdaptItemFontSize();
	AdaptCraftItemCount();
//	CreateThread((LPTHREAD_START_ROUTINE)ThrMouseTips);
//	CreateThread((LPTHREAD_START_ROUTINE)ThrGameChecks);

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

void DrawNewVoxelCfg(bool firstDraw) {
	static const int title_top = uiy * 0.15;
	static const int text_top = uiy * 0.4;

	cls();

	setfont(uid ui_nvcfg_title_fs,0,"FixedSys");
	setcolor(YELLOW);
	midpxprinty("维度初始配置",title_top,0,uix,firstDraw?100:0);
	setfont(uid ui_nvcfg_text_fs,0,"FixedSys");
	setcolor(LIGHTGRAY);
	pxprint(uix/2 - (nvcfg::seed_box_w+100+uid ui_nvcfg_text_fs*5)/2,text_top,"维度随机种子:",firstDraw?80:0);
	pxprint(uix/2 - (nvcfg::pname_box_w+100+uid ui_nvcfg_text_fs*3)/2,text_top+uid ui_nvcfg_text_fs+30,"你的名字:",firstDraw?90:0);
	if(firstDraw || !nvcfg::seed_box->isvisable() || !nvcfg::pname_box->isvisable()) {
		nvcfg::seed_box->visable(true);
		nvcfg::pname_box->visable(true);
	}

	if(!nvcfg::tip.empty()) {
		setfont(uid ui_nvcfg_text_fs,0,"FixedSys");
		setcolor(LIGHTRED);
		midpxprinty(nvcfg::tip,uiy*0.55);
	}

	setfont(uid ui_nvcfg_btn_fs,0,"FixedSys");
	if(uid ui_nvcfg_pre_id == 0)
		setcolor(PINK);
	else
		setcolor(LIGHTCYAN);
	uid rt_nvcfg_done = midpxprinty("[Ctrl+S] 开始构建",uiy*0.77,0,uix,firstDraw?30:0);
	if(uid ui_nvcfg_pre_id == 1)
		setcolor(PINK);
	else
		setcolor(LIGHTCYAN);
	uid rt_nvcfg_cancel = midpxprinty("[Ctrl+W] 取消",uiy*0.77+uid ui_nvcfg_btn_fs*2,0,uix,firstDraw?30:0);

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
					bool ret = CheckNewVoxelCfg(seed,pname);
					if(!ret)
						continue;
					nvcfg::DeleteBoxes();
					return true;
				} else
					uid ui_nvcfg_pre_id = 0;
			} else if(InRect(msg.x,msg.y,uid rt_nvcfg_cancel)) {
				if(msg.is_left() && msg.is_up()) {
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
					bool ret = CheckNewVoxelCfg(seed,pname);
					if(!ret)
						continue;
					nvcfg::DeleteBoxes();
					return true;
				} else if(K('W')) {
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
string GetPlayerShape() {
	return p.shape;
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
void SaveWorld() {
	DebugLog("【保存】保存世界数据:"+WORLDDATA_PATH);
	fstream fout(WORLDDATA_PATH,ios::out);

	fout<<"[WorldBlocks]"<<endl;
	optprint(fout, "WorldWidth", world.map_x);
	optprint(fout, "WorldHeight", world.map_y);
	optprint(fout, "Time", world.time);
	long i=0;
	for(short _y = 0; _y < world.map_y; ++_y)
		for(short _x = 0; _x < world.map_x; ++_x) {
			const Block& bcref = world.GetBlock(_x,_y,"SaveWorld");
			fout<<bcref.id<<" "<<bcref.dur<<" "<<bcref.tag.ToString()<<"	";
			/*if(i % SAVEWORLD_NEWLINE_BLOCK_CNT == 0)
				fout<<endl;		//定期换行 */
			++i;
		}
	fout<<endl;
	fout<<"[Entities]"<<endl;
	for(long i = 0; i < entities.size(); ++i)
	{
		fout<<entities.at(i).id<<" "<<entities.at(i).uuid<<" "<<entities.at(i).ai
		    <<" "<<entities.at(i).x<<" "<<entities.at(i).y<<" "<<entities.at(i).status<<" "<<entities.at(i).dir
		    <<" "<<entities.at(i).hp<<" "<<entities.at(i).tag.ToString()<<endl;
	}

	fout.close();
}
void ReadWorld() {
	if(!ExistFile(WORLDDATA_PATH)) {
		ErrorLogTip(NULL,"世界存档丢失!","ReadWorld");
		return;
	}
	world.Clear();

	DebugLog("【读取】读取世界数据:"+WORLDDATA_PATH);
	fstream fin(WORLDDATA_PATH,ios::in);
	string tmp;
	fin>>tmp;	//Header
	optinput(fin,world.map_x);
	optinput(fin,world.map_y);
	optinput(fin,world.time);
	while(!fin.eof() && tmp != "[Entities]") {
		string tmp;
		fin>>tmp;
		if(tmp == "[Entities]")
			break;
		Block b;
		b.id = Convert<string,USHORT>(tmp);
		fin>>b.dur;
		fin>>tmp;
		if(!tmp.empty())
			b.tag = ParseJSON(tmp,"ReadWorld");
		b.AdjustShape();
		world.PushBlock(b);
	}
	DebugLog("【读取】世界方块读取完毕。世界大小W="+ToString(world.map_x)+" H="+ToString(world.map_y)+" 方块总数="+ToString(world.blocks.size()));
	while(!fin.eof() && fin)
	{
		Entity et;
		fin>>et.id;
		if(fin.eof() || !fin)
			break;
		fin>>et.uuid>>et.ai>>et.x>>et.y>>et.status>>et.dir>>et.hp;
		string tmp;
		fin>>tmp;
		et.tag = ParseJSON(tmp,"ReadWorld");
		entities.push_back(et);
	}
	if(!entities.empty())
		DebugLog("【读取】世界实体读取完毕，总数="+ToString(entities.size()));
	fin.close();
}

#define BIOME_DEF_CN_NAME "未知群系"
#define BIOME_DEF_EN_NAME "UnknownBiome"
class BiomeData {	//生物群系数据
	public:
		BIOME_ID id;
		string cn_name;
		string en_name;

		vector<Weight<BLOCK_ID>> blocks;
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

		biomedata.push_back(bdt);
		++i;
	}
}

BLOCK_ID GetBiomeRandomBlock(BIOME_ID biome) {
	BLOCK_ID bid=0;
	bid = biomedata.at(USHORT(biome)).RandomBlock();
	return bid;
}
const short _nxtAround[4][2] = {{1,0},{0,1},{-1,0},{0,-1}};
void bfsRenderWorld(BIOME_ID biome, long cnt, long x, long y) {
	if(cnt <= 0 || world.GetBlock(x,y).biome == biome)
		return;		//剪掉顶芽

	BLOCK_ID bid = GetBiomeRandomBlock(biome);
	world.SetBlock(x,y,bid,biome);

	long _x,_y;
	for(int i=0; i<4; i++) {
		_x=x,_y=y;
		_x += _nxtAround[i][0];
		_y += _nxtAround[i][1];
		if(_x < 0 || _x >= world.map_x || _y < 0 || _y >= world.map_y)
			continue;		//越界剪枝

		bfsRenderWorld(biome,--cnt,_x,_y);	//递归之
	}
	return;
}
short bfs_giveup_percentage = 10;
void bfsRenderWorld_unstable(BIOME_ID biome, long cnt, long x, long y) {
	if(cnt <= 0 || world.GetBlock(x,y).biome == biome)
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
		if(_x < 0 || _x >= world.map_x || _y < 0 || _y >= world.map_y)
			continue;		//越界剪枝

		bfsRenderWorld_unstable(biome,--cnt,_x,_y);	//递归之
	}
	return;
}

#define DEF_MAIN_PATH_DIR_WEIGHT 6
int main_path_dir_weight = DEF_MAIN_PATH_DIR_WEIGHT;
#define MAIN_PATH_DIR_WEIGHT main_path_dir_weight
void pathCreater(BLOCK_ID bid, DIR face, long cnt, long x, long y) {
	//小径开创者 (单向)
	if(x < 0 || x >= world.map_x || y < 0 || y >= world.map_y)
		return;		//越界剪枝
	if(cnt <= 0 || world.GetBlock(x,y,"pathCreater").id == bid)
		return;				//剪枝2

	world.SetBlock(x,y,bid);

	DIR dir;

	dir = face;

	if(face == RIGHT) {
		if(Percent(main_path_dir_weight))
			dir = face;
		else {
			dir = Choice({RIGHTDOWN,RIGHTUP});
		}
	} 
	else if(face == DOWN) {
		if(Percent(main_path_dir_weight))
			dir = face;
		else {
			dir = Choice({RIGHTDOWN,LEFTDOWN});
		}
	}else if(face == LEFT) {
		if(Percent(main_path_dir_weight))
			dir = face;
		else {
			dir = Choice({LEFTDOWN,LEFTUP});
		}
	}
	else if(face == UP) {
		if(Percent(main_path_dir_weight))
			dir = face;
		else {
			dir = Choice({RIGHTUP,LEFTUP});
		}
	}else if(face == RIGHTUP) {
		if(Percent(main_path_dir_weight))
			dir = face;
		else {
			dir = Choice({RIGHT,UP});
		}
	}else if(face == RIGHTDOWN) {
		if(Percent(main_path_dir_weight))
			dir = face;
		else {
			dir = Choice({RIGHT,DOWN});
		}
	}else if(face == LEFTDOWN) {
		if(Percent(main_path_dir_weight))
			dir = face;
		else {
			dir = Choice({LEFT,DOWN});
		}
	}else if(face == LEFTUP) {
		if(Percent(main_path_dir_weight))
			dir = face;
		else {
			dir = Choice({UP,LEFT});
		}
	}
	DirOffsetPos(x,y,dir);
	pathCreater(bid,dir,--cnt,x,y);
}
void SpawnNaturalOres()
{	//生成自然矿石 
    XCPT_L
	repeat(RandomRange(220,366))
	{	//石
		long x,y;
		do{
			x = RandomRange(30,world.map_x-30);
			y = RandomRange(30,world.map_y-30);
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
			x = RandomRange(30,world.map_x-30);
			y = RandomRange(30,world.map_y-30);
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
void SpawnLongWalls(void)
{
	int rx,ry;
	//#2生成随机长城
	main_path_dir_weight = 3;
	//①木栅栏
	repeat(RandomRange(55,150)) {
		rx = RandomRange(200,world.map_x-200,false,false);
		ry = RandomRange(200,world.map_y-200,false,false);
		pathCreater(42,RandomDir4(),RandomRange(30,99),rx,ry);
	}
	main_path_dir_weight = 1;
	//②石砖
	repeat(RandomRange(10,27)) {
		rx = RandomRange(200,world.map_x-200,false,false);
		ry = RandomRange(200,world.map_y-200,false,false);
		pathCreater(22,RandomDir4(),RandomRange(99,67),rx,ry);
	}
	//③木头
	repeat(RandomRange(20,40)) {
		rx = RandomRange(200,world.map_x-200,false,false);
		ry = RandomRange(200,world.map_y-200,false,false);
		pathCreater(13,RandomDir4(),RandomRange(36,86),rx,ry);
	}
	//④铁栏杆 
	main_path_dir_weight = 7;
	repeat(RandomRange(40,70)) {
		rx = RandomRange(200,world.map_x-200,false,false);
		ry = RandomRange(200,world.map_y-200,false,false);
		pathCreater(45,RandomDir4(),RandomRange(25,50),rx,ry);
	}
	//⑤金栏杆 
	repeat(RandomRange(4,14)) {
		rx = RandomRange(300,world.map_x-200,false,false);
		ry = RandomRange(300,world.map_y-200,false,false);
		pathCreater(46,RandomDir4(),RandomRange(10,30),rx,ry);
	}
	main_path_dir_weight = 6;
	//⑥仙人掌篱笆 
	repeat(RandomRange(16,44)) {
		rx = RandomRange(200,world.map_x-200,false,false);
		ry = RandomRange(200,world.map_y-200,false,false);
		pathCreater(72,RandomDir4(),RandomRange(12,80),rx,ry);
	}
}
void SpawnRuins(void) {	//生成遗迹
	//#1 生成天然小房间
	const vector<BLOCK_ID> wall_list	//墙体预设
	{4/*Stone*/,4,13/*log*/,14/*plank*/,14,14,20/*ice*/,22/*stonebrick*/,22,23/*hellstonebrick*/,};
	int wall,x,y;
	repeat(RandomRange(120,240,true,true)) {
		//$定位
		do {
			x=RandomRange(40,world.map_x-60,true,true);
			y=RandomRange(50,world.map_y-60,true,true);
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
}
void DensityAreaSetBlock(int rpt_min,int rpt_max, int spr_min, int spr_max, int den_min, int den_max, BLOCK_ID id)
{
	XCPT_L
	repeat(RandomRange(rpt_min,rpt_max))
	{
			int rx=RandomRange(200,world.map_x-200,true,true);
			int ry=RandomRange(200,world.map_y-200,true,true);
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
	DensityAreaSetBlock(3,6,6,35,20,85,EnNameToBlock("reeds"));	//芦苇 
	DensityAreaSetBlock(30,60,10,30,2,11,EnNameToBlock("pink_flower"));	//粉色花 
	DensityAreaSetBlock(22,40,17,40,19,36,EnNameToBlock("scarlet_grass"));	//猩红草 
	DensityAreaSetBlock(9,19,24,50,3,25,EnNameToBlock("golden_flower"));	//金色花 
	DensityAreaSetBlock(10,13,34,45,15,45,EnNameToBlock("blue_grass"));	//碧蓝草 
	DensityAreaSetBlock(24,33,34,75,3,7,EnNameToBlock("ice_lotus"));	//冰雪莲 
	XCPT_R
}

#define NV_DELAY_MS 100
#define nvde api_sleep(NV_DELAY_MS);
DWORD ThrCreateNewVoxel(LPVOID none) {
	XCPT_L
	DebugLog("[ThrCreateNewVoxel] 开始创建维度");
	g_nv_status=0;
	DebugLog("[ThrCreateNewVoxel] 随机地图大小...");

	world.Clear();
	world.map_x = RandomRange(MAP_X_MIN,MAP_X_MAX);
	world.map_y = RandomRange(MAP_Y_MIN,MAP_Y_MAX);
	world.time = World::Morning;
	api_sleep(500);

	g_nv_status=1;
	DebugLog("[ThrCreateNewVoxel] 生成缺省地形...");
	for(int _y = 0; _y < world.map_y; _y ++)
		for(int _x = 0; _x < world.map_x; _x ++) {
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
			b.Reset(_id);
			world.PushBlock(b);
		}
	DebugLog("[ThrCreateNewVoxel] 生成沙漠...");
	g_nv_status = 2;

	for(short j = 0; j < RandomRange(6,12); ++j) {
		long _x,_y;
		_x = RandomRange(10,world.map_x-10);
		_y = RandomRange(10,world.map_y-10);
		bfsRenderWorld(biome_desert,RandomRange(100,300),_x,_y);
	}
	nvde
	DebugLog("[ThrCreateNewVoxel] 生成松树林...");
	g_nv_status = 3;

	for(short j = 0; j < RandomRange(9,19); ++j) {
		long _x,_y;
		_x = RandomRange(10,world.map_x-10);
		_y = RandomRange(10,world.map_y-10);
		bfsRenderWorld(biome_pine_forest,RandomRange(40,145),_x,_y);
	}
	nvde
	DebugLog("[ThrCreateNewVoxel] 生成杨树林...");
	g_nv_status = 4;

	for(short j = 0; j < RandomRange(4,9); ++j) {
		long _x,_y;
		_x = RandomRange(30,world.map_x-30);
		_y = RandomRange(30,world.map_y-30);
		bfsRenderWorld(biome_poplar_forest,RandomRange(40,133),_x,_y);
	}
	nvde
	DebugLog("[ThrCreateNewVoxel] 生成银杏林...");
	g_nv_status = 5;

	for(short j = 0; j < RandomRange(2,6); ++j) {
		long _x,_y;
		_x = RandomRange(30,world.map_x-30);
		_y = RandomRange(30,world.map_y-30);
		bfsRenderWorld(biome_ginkgo_forest,RandomRange(63,165),_x,_y);
	}
	nvde
	DebugLog("[ThrCreateNewVoxel] 生成沼泽...");
	g_nv_status = 6;
	bfs_giveup_percentage = 20;

	for(short j = 0; j < RandomRange(5,8); ++j) {
		long _x,_y;
		_x = RandomRange(5,world.map_x-5);
		_y = RandomRange(5,world.map_y-5);
		bfsRenderWorld_unstable(biome_swamp,RandomRange(89,487),_x,_y);
	}
	nvde
	DebugLog("[ThrCreateNewVoxel] 生成樟树林...");
	g_nv_status = 7;

	for(short j = 0; j < RandomRange(5,11); ++j) {
		long _x,_y;
		_x = RandomRange(60,world.map_x-60);
		_y = RandomRange(60,world.map_y-60);
		bfsRenderWorld(biome_camphor_forest,RandomRange(90,133),_x,_y);
	}
	nvde
	DebugLog("[ThrCreateNewVoxel] 生成岩壁...");
	g_nv_status = 8;
	bfs_giveup_percentage = 3;

	for(short j = 0; j < RandomRange(9,17); ++j) {
		long _x,_y;
		_x = RandomRange(5,world.map_x-5);
		_y = RandomRange(5,world.map_y-5);
		bfsRenderWorld_unstable(biome_stones,RandomRange(120,444),_x,_y);
	}
	nvde
	DebugLog("[ThrCreateNewVoxel] 生成石林...");
	g_nv_status = 9;
	bfs_giveup_percentage = 10;

	for(short j = 0; j < RandomRange(5,7); ++j) {
		long _x,_y;
		_x = RandomRange(5,world.map_x-5);
		_y = RandomRange(5,world.map_y-5);
		bfsRenderWorld_unstable(biome_rocks,RandomRange(60,177),_x,_y);
	}
	nvde
	DebugLog("[ThrCreateNewVoxel] 生成草原...");
	g_nv_status = 10;

	for(short j = 0; j < RandomRange(6,9); ++j) {
		long _x,_y;
		_x = RandomRange(5,world.map_x-5);
		_y = RandomRange(5,world.map_y-5);
		bfsRenderWorld(biome_grass,RandomRange(100,244),_x,_y);
	}
	nvde
	DebugLog("[ThrCreateNewVoxel] 生成丛林...");
	g_nv_status = 11;

	for(short j = 0; j < RandomRange(5,11); ++j) {
		long _x,_y;
		_x = RandomRange(55,world.map_x-55);
		_y = RandomRange(55,world.map_y-55);
		bfsRenderWorld(biome_jungle,RandomRange(352,744),_x,_y);
	}
	nvde
	DebugLog("[ThrCreateNewVoxel] 生成竹林...");
	g_nv_status = 12;
	bfs_giveup_percentage = 20;

	for(short j = 0; j < RandomRange(6,8); ++j) {
		long _x,_y;
		_x = RandomRange(55,world.map_x-55);
		_y = RandomRange(55,world.map_y-55);
		bfsRenderWorld_unstable(biome_bamboo_forest,RandomRange(90,297),_x,_y);
	}
	nvde
	DebugLog("[ThrCreateNewVoxel] 生成冰原...");
	g_nv_status = 13;

	for(short j = 0; j < RandomRange(5,10); ++j) {
		long _x,_y;
		_x = RandomRange(55,world.map_x-55);
		_y = RandomRange(55,world.map_y-55);
		bfsRenderWorld(biome_iceland,RandomRange(99,346),_x,_y);
	}
	nvde
	DebugLog("[ThrCreateNewVoxel] 生成针叶林...");
	g_nv_status = 14;

	for(short j = 0; j < RandomRange(8,14); ++j) {
		long _x,_y;
		_x = RandomRange(5,world.map_x-5);
		_y = RandomRange(5,world.map_y-5);
		bfsRenderWorld(biome_taiga,RandomRange(333,666),_x,_y);
	}
	nvde
	DebugLog("[ThrCreateNewVoxel] 生成苹果树林...");
	g_nv_status = 15;

	for(short j = 0; j < RandomRange(4,8); ++j) {
		long _x,_y;
		_x = RandomRange(5,world.map_x-5);
		_y = RandomRange(5,world.map_y-5);
		bfsRenderWorld(biome_apple_forest,RandomRange(90,444),_x,_y);
	}
	
	nvde
	DebugLog("[ThrCreateNewVoxel] 生成山区...");
	g_nv_status = 16;
	bfs_giveup_percentage = 40;

	for(short j = 0; j < RandomRange(22,43); ++j) {
		long _x,_y;
		_x = RandomRange(25,world.map_x-25);
		_y = RandomRange(35,world.map_y-35);
		bfsRenderWorld_unstable(biome_mountain,RandomRange(34,67),_x,_y);
	}
	nvde
	DebugLog("[ThrCreateNewVoxel] 埋藏矿石...");
	g_nv_status = 17;
	SpawnNaturalOres();
	
	nvde
	DebugLog("[ThrCreateNewVoxel] 生成小路...");
	g_nv_status = 18;
	for(short j = 0; j < RandomRange(1200,2400); ++j) {
		long _x,_y;
		_x = RandomRange(5,world.map_x-5);
		_y = RandomRange(5,world.map_y-5);
		pathCreater(0,RandomDir4(),RandomRange(10,320),_x,_y);
	}
	nvde
	SpawnLongWalls();
	
	nvde
	DebugLog("[ThrCreateNewVoxel] 生成溪流...");
	g_nv_status = 19;
	main_path_dir_weight = 4;
	for(short j = 0; j < RandomRange(360,600); ++j) {
		long _x,_y;
		_x = RandomRange(5,world.map_x-5);
		_y = RandomRange(5,world.map_y-5);
		pathCreater(EnNameToBlock("water"),RandomDir4(),RandomRange(11,56),_x,_y);
	}
	nvde
	DebugLog("[ThrCreateNewVoxel] 生成河流...");
	g_nv_status = 20;
	for(short j = 0; j < RandomRange(120,230); ++j) {
		long _x,_y;
		main_path_dir_weight = Choice({8,8,8,2,5,7,9});
		_x = RandomRange(65,world.map_x-65);
		_y = RandomRange(65,world.map_y-65);
		pathCreater(EnNameToBlock("water"),RandomDir4(),RandomRange(110,760),_x,_y);
	}
	main_path_dir_weight = 9;
	for(short j = 0; j < RandomRange(60,90); ++j) {
		long _x,_y;
		main_path_dir_weight = Choice({9,8,8,8,4});
		_x = RandomRange(65,world.map_x-65);
		_y = RandomRange(65,world.map_y-65);
		pathCreater(EnNameToBlock("water"),RandomDir4(),RandomRange(3780,5360),_x,_y);
	}
	nvde
	DebugLog("[ThrCreateNewVoxel] 生成各种植物...");
	g_nv_status = 21;
	SpawnPlants();
	nvde
	long _x,_y;
	main_path_dir_weight = 3;
	_x = RandomRange(65,world.map_x-65);
	_y = RandomRange(65,world.map_y-65);
	pathCreater(EnNameToBlock("water"),RandomDir4(),RandomRange(12780,55360),_x,_y);
	nvde
	DebugLog("[ThrCreateNewVoxel] 生成湖泊...");
	g_nv_status = 22;
	bfs_giveup_percentage = 25;
	for(short j = 0; j < RandomRange(14,28); ++j) {
		long _x,_y;
		_x = RandomRange(25,world.map_x-25);
		_y = RandomRange(35,world.map_y-35);
		bfsRenderWorld_unstable(biome_lake,Choice({8,10,24,33,36,56,91,140}),_x,_y);
	}
	nvde
	DebugLog("[ThrCreateNewVoxel] 生成遗迹...");
	g_nv_status = 23;
	SpawnRuins();
	nvde
	DebugLog("[ThrCreateNewVoxel] 生成海洋...");
	g_nv_status = 24;
	/*for(short j = 0; j < RandomRange(2,4); ++j)
	{
		long _x,_y;
		_x = RandomRange(5,world.map_x-5);
		_y = RandomRange(5,world.map_y-5);
		bfsRenderWorld(biome_ocean,RandomRange(666,1200),_x,_y);
	}*/
#define OCEAN_WIDTH 30
	for(long y = 0; y < world.map_y; ++y)
		for(long x = 0; x < world.map_x; ++x) {
			if(x < OCEAN_WIDTH || x >= world.map_x - OCEAN_WIDTH
			        || y < OCEAN_WIDTH || y >= world.map_y - OCEAN_WIDTH) {
//				DebugLog("x="+ToString(x)+" y="+ToString(y));
				world.SetBlock(x,y,EnNameToBlock("water"),biome_ocean,"ThrCreateNewVoxel");
			}
		}
	DebugLog("[ThrCreateNewVoxel] 保存维度至文件...");
	g_nv_status = 25;
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
			return "Putting vast Deserts...";
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
			return "Putting large grasslands...";
		case 11:
			return "Spawning Jungles...";
		case 12:
			return "Putting Bamboo Forests...";
		case 13:
			return "Creating icelands...";
		case 14:
			return "Spawning Taiga...";
		case 15:
			return "Planting Apple Trees...";
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
			return "Spawning various plants...";
		case 22:
			return "Creating Ruins...";
		case 23:
			return "Pouring lakes...";
		case 24:
			return "Creating oceans...";
		case 25:
			return "Saving the Voxel";
		default:
			return "*** Mysterious Process ***";
	}
	return nullptr;
}
PIMAGE newVoxelMap=nullptr;
void DrawNewVoxelCreation(bool firstDraw=false) {
	static const int text_fs = 65;
	static const int proc_fs = 50;
	cls();

	setfont(text_fs,0,"FixedSys");
	setcolor(YELLOW);
	midpxprinty("正在创建新维度",uiy*0.75,0,uix,firstDraw?80:0);

	string sProc = GetNVCreationProcess(g_nv_status);
	setfont(proc_fs,0,"FixedSys");
	setcolor(LIGHTGRAY);
	midpxprinty(sProc,uiy*0.75+text_fs+10,0,uix,0);
	
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
	MsgSndTip();
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
	/*midpxprinty("种子="+ToString(seed)+"玩家名="+pname,uiy*0.5);
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
	setfont(65,0,"FixedSys");
	setcolor(WHITE);
	midpxprinty("正在赋予"+pname+"生命",uiy*0.45,0,uix,20);
	DebugLog("玩家名为"+pname+"，创建玩家数据中");
	CreateNewPlayer(pname);	//Saving inside
	delay(500);

	cls();
	setcolor(LIGHTGREEN);
	midpxprinty("维度构建完成",uiy*0.45,0,uix,40);
	delay(1000);
	cls();

	return true;
}
void DrawCreationChoice(bool firstDraw=false) {
	cls();

	setfont(55,0,"FixedSys");
	setcolor(YELLOW);
	midpxprinty("没有找到旧存档。",uiy*0.28,0,uix,firstDraw?50:0);
	if(uid ui_play_cc_pre_id == 0)
		setcolor(LIGHTRED);
	else
		setcolor(LIGHTCYAN);
	uid rt_play_create_new = midpxprinty("[Space: 开始新篇章]",uiy*0.45,0,uix,firstDraw?10:0);
	if(uid ui_play_cc_pre_id == 1)
		setcolor(LIGHTRED);
	else
		setcolor(LIGHTCYAN);
	uid rt_play_giveup = midpxprinty("[Esc: 返回]",uiy*0.45+55*2,0,uix,firstDraw?7:0);

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
				return CreateNewVoxel();
			} else if(K(VK_ESCAPE)) {
				DebugLog("放弃创建新维度。");
				return false;
			}
		} else if(mousemsg()) {
			mouse_msg msg = getmouse();
			if(InRect(msg.x,msg.y, uid rt_play_create_new)) {
				if(msg.is_left() && msg.is_up()) {
					return CreateNewVoxel();
				} else
					uid ui_play_cc_pre_id = 0;
			} else if(InRect(msg.x,msg.y, uid rt_play_giveup)) {
				if(msg.is_left() && msg.is_up()) {
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
	setfont(50,0,"FixedSys");
	setcolor(LIGHTGRAY);
	midpxprinty(tip,uiy*0.78);
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
		setfont(60,0,"FixedSys");
		setcolor(YELLOW);
		midpxprinty("读取存档中",uiy*0.35,0,uix,50);
		ReadSave();
		cls();
		DrawTip(tip);

		setfont(60,0,"FixedSys");
		setcolor(MAGENTA);
		midpxprinty("加入世界中",uiy*0.35,0,uix,50);
		delay(1000);

		cls();
	}
	InGame();
}
void Leave() {
	DebugLog("离开游戏...");
	closegraph();
	exit(0);
}

void SaveOptions() {
	DebugLog("【保存】保存游戏配置中:"+OPTIONS_PATH);
	fstream fout(OPTIONS_PATH,ios::out);
	fout<<"[Options]"<<endl;
	optprint(fout, "Version", CURRENT_VERSION);
	optprint(fout, "Music", options.music_switch);
	optprint(fout, "BlockTagShown", options.blocktag_switch);
	optprint(fout, "DebugInfoShown", options.show_debug_info);
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
	optinput(fin, options.blocktag_switch);
	optinput(fin, options.show_debug_info);
	fin.close();
}

#define OPT_COL_COLOR WHITE
#define OPT_PRE_COL_COLOR EGERGB(74,231,54)
#define OPT_CUR_COL_COLOR RED
void DrawOptions() {
	static const int ui_opt_col_left = uix * 0.11;
	static const int ui_opt_col_top = uiy * 0.28;
	static const int ui_opt_col_fs = 50;
	static const int ui_opt_col_gap = 50;
	static const int ui_opt_content_left = ui_opt_col_left+ui_opt_col_fs*2+250;
	static const int ui_opt_content_right = ui_opt_content_left + uix * 0.5;
	static const int ui_opt_content_top = uiy * 0.15f;
	static const int ui_opt_esc_left = 60;
	static const int ui_opt_esc_top = uiy * 0.78;

	cls();

	setfont(55,0,"FixedSys");
	if(uid ui_opt_col_pre_id == -2)
		setcolor(LIGHTRED);
	else
		setwcolor(CORNSILK);
	uid rt_opt_esc = pxprint(ui_opt_esc_left,ui_opt_esc_top,"[Esc:保存并返回]");

	setfont(uid ui_subtitle_fs,0,"FixedSys");
	setwcolor(YELLOW);
	pxprint(ui_opt_col_left,ui_opt_col_top - uid ui_subtitle_fs - 55,"选项");

	setfont(ui_opt_col_fs,0,"FixedSys");
	for(short i = 0; i < uid ui_opt_cols.size(); ++i) {
		if(i == uid ui_opt_col_id) {
			setcolor(OPT_CUR_COL_COLOR);
			pxprint(ui_opt_col_left-ui_opt_col_fs+2, ui_opt_col_top + (ui_opt_col_fs+ui_opt_col_gap)*i,"》"+uid ui_opt_cols.at(i));
		} else if(i == uid ui_opt_col_pre_id) {
			setcolor(OPT_PRE_COL_COLOR);
			pxprint(ui_opt_col_left-2*(ui_opt_col_fs-2), ui_opt_col_top + (ui_opt_col_fs+ui_opt_col_gap)*i,"·  "+uid ui_opt_cols.at(i));
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
	setfont(40,0,"System");
	setcolor(CYAN);
	for(short j = 0; j < (uiy * 0.7 / 40); ++j)
		xyprintf(ui_opt_content_left-100,uiy*0.15+40*j,"!");

	setfont(55,0,"FixedSys");
	setwcolor(PINK);
	midpxprinty("—————— "+uid ui_opt_cols.at(uid ui_opt_col_id)+" ——————",ui_opt_content_top+10,ui_opt_content_left,ui_opt_content_right);

	if(uid ui_opt_col_id == 0) {
		//存档
		setfont(45,0,"FixedSys");
		if(uid ui_opt_col_pre_id == -3)
			setwcolor(PINK);
		else
			setcolor(LIGHTCYAN);
		uid rt_opt_save_opendir = midpxprinty("[Space:打开存档文件夹]",ui_opt_content_top+200,ui_opt_content_left,ui_opt_content_right);

		if(uid ui_opt_col_pre_id == -4)
			setwcolor(PINK);
		else
			setcolor(LIGHTCYAN);
		uid rt_opt_save_delete = midpxprinty("[Delete:删除存档]",ui_opt_content_top+400,ui_opt_content_left,ui_opt_content_right);
	} else if(uid ui_opt_col_id == 1) {
		//控制
		static const int _op_top = ui_opt_content_top+90;
		static const int ctrl_fs = 38;
		static const int addi = ctrl_fs+4;
		setfont(ctrl_fs,0,"FixedSys");
		setwcolor(ORANGE);
		midpxprinty("★基本操作★",_op_top,ui_opt_content_left,ui_opt_content_right);
		setcolor(LIGHTGRAY);
		midpxprinty("AWSD: 移动玩家        ↑←↓→1~0: 背包物品选择",_op_top+addi,ui_opt_content_left,ui_opt_content_right);
		midpxprinty("J: 使用当前物品    Backspace: 丢弃一个物品",_op_top+addi*2,ui_opt_content_left,ui_opt_content_right);
		midpxprinty("K: 操作面前方块        Shift+Backspace: 丢弃一组物品",_op_top+addi*3,ui_opt_content_left,ui_opt_content_right);
		midpxprinty("Ctrl:开启/关闭方块提示 Delete: 删除一个物品",_op_top+addi*4,ui_opt_content_left,ui_opt_content_right);
		midpxprinty("Esc: 游戏主菜单   Shift+Delete: 删除一组物品",_op_top+addi*5,ui_opt_content_left,ui_opt_content_right);
		midpxprinty("上述大部分操作可以用鼠标代替:",_op_top+addi*6,ui_opt_content_left,ui_opt_content_right);
		midpxprinty("方块上左键：镐/斧挖掘方块/砍树   物品上左键：选择物品",_op_top+addi*7,ui_opt_content_left,ui_opt_content_right);
		midpxprinty("方块上右键：操作方块   物品上右键：取物品",_op_top+addi*8,ui_opt_content_left,ui_opt_content_right);
		setwcolor(ORANGE);
		midpxprinty("★合成操作★",_op_top+addi*9,ui_opt_content_left,ui_opt_content_right);
		setcolor(LIGHTGRAY);
		midpxprinty("< > 以及鼠标左键:选择合成项目",_op_top+addi*10,ui_opt_content_left,ui_opt_content_right);
		midpxprinty("Enter或鼠标右键:合成一次所选项目",_op_top+addi*11,ui_opt_content_left,ui_opt_content_right);
	} else if(uid ui_opt_col_id == 2) {
		//音乐
		setfont(45,0,"FixedSys");
		if(uid ui_opt_col_pre_id == -5)
			setwcolor(PINK);
		else
			setcolor(LIGHTGREEN);
		uid rt_opt_mus_switch = midpxprinty("音乐      ",ui_opt_content_top+220,ui_opt_content_left,ui_opt_content_right);
		string s="     ";
		color_t clr=WHITE;
		s += GetSwitch(options.music_switch,clr,DS_GRAY_GREEN);
		setcolor(clr);
		midpxprinty(s,ui_opt_content_top+220,ui_opt_content_left,ui_opt_content_right);
	}
	const int _ver_fs = 50;
	setwcolor(LIGHTGRAY);
	setfont(_ver_fs,0,"FixedSys");
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
		ss<<"cmd /c del "<<LOCAL_SAVE_DIR<<"*.dat /q";
		WinExec(ss.str().c_str(),SW_HIDE);
		MsgSndTip();
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
				ClampA<short>(uid ui_opt_col_id,0,uid ui_opt_cols.size()-1);
			} else if((K(VK_DOWN) || K('S'))
			          && uid ui_opt_col_id < uid ui_opt_cols.size()-1) {
				uid ui_opt_col_id++;
				ClampA<short>(uid ui_opt_col_id,0,uid ui_opt_cols.size()-1);
			} else if((K(VK_TAB) && K(VK_CONTROL))) {
				//Ctrl+Tab
				uid ui_opt_col_id = (uid ui_opt_col_id + 1) % uid ui_opt_cols.size();
			} else if(K(VK_ESCAPE)) {
				//Esc
				SaveOptions();
				break;
			}

			if(uid ui_opt_col_id == 0) {
				if(K(VK_SPACE)) {
					ShowSaveInExplorer();
				} else if(K(VK_DELETE)) {
					DeleteSave();
				}
			} else if(uid ui_opt_col_id == 1) {
				//none
			} else if(uid ui_opt_col_id == 2) {
				if(K(VK_SPACE)) {
					options.music_switch = !options.music_switch;
					delay(50);
				}
			}
			lastKey = clock();
		}
		if(mousemsg()) {
			mouse_msg msg = getmouse();
			uid ui_opt_col_pre_id = -1;	//defvalue
			for(short i = 0; i < uid rt_opt_cols.size(); ++i) {
				//鼠标选择左侧选项卡
				if(InRect(msg.x,msg.y,uid rt_opt_cols[i])) {
					if(msg.is_left() && msg.is_up()) //选择
						uid ui_opt_col_id = i;
					else	//提前
						uid ui_opt_col_pre_id = i;
				}
			}
			if(InRect(msg.x,msg.y,uid rt_opt_esc)) {
				//保存并返回
				if(msg.is_left() && msg.is_up()) {
					SaveOptions();
					break;
				} else
					uid ui_opt_col_pre_id = -2;
			} else if(uid ui_opt_col_id == 0) {
				//Save
				if(InRect(msg.x,msg.y,uid rt_opt_save_opendir)) {
					if(msg.is_left() && msg.is_up())
						ShowSaveInExplorer();
					else
						uid ui_opt_col_pre_id = -3;
				} else if(InRect(msg.x,msg.y,uid rt_opt_save_delete)) {
					if(msg.is_left() && msg.is_up())
						DeleteSave();
					else
						uid ui_opt_col_pre_id = -4;
				}
			} else if(uid ui_opt_col_id == 2) {
				//Music
				if(InRect(msg.x,msg.y,uid rt_opt_mus_switch)) {
					if(msg.is_left() && msg.is_up())
						options.music_switch = !options.music_switch;
					else
						uid ui_opt_col_pre_id = -5;
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
	setfont(180,0,"Courier");
	setcolor(YELLOW);
	xyprintf(100,uiy*0.3,"DarkVoxel");
	setfont(200,0,"System");
	setcolor(LIGHTGREEN);
	xyprintf(9*180/2-50,uiy*0.3+95,"2");

	const int _ver_left = uix*0.05f;
	const int _ver_fs = 50;
	const int _ver_top = uiy - _ver_fs*2-200;
	setcolor(LIGHTGRAY);
	setfont(_ver_fs,0,"FixedSys");
	xyprintf(_ver_left,_ver_top,("V"+strxhead(CURRENT_VERSION)).c_str());
	pxprint(_ver_left,_ver_top+_ver_fs,"由 Wormwaker 倾情打造");

	const int _right_dist = uix * 0.35f;
	const int _top = uiy * 0.35f;
	const int _gap = 200;
	const int _btn_left = uix - _right_dist;
	const int _btn_right = uix;
	setfont(80,0,"FixedSys");
	DMUI_BTN_COLOR_ADAPT(0)
	uid rt_splay = midpxprinty("[Space:单人]", _top, _btn_left, _btn_right);
	DMUI_BTN_COLOR_ADAPT(1)
	uid rt_opt = midpxprinty("[O:选项]", _top + _gap, _btn_left, _btn_right);
	DMUI_BTN_COLOR_ADAPT(2)
	uid rt_leave = midpxprinty("[Esc:离去]", _top + _gap * 2, _btn_left, _btn_right);

	delay_fps(60);
}
#define MUI_DRAW_CD 60
void EnterMainUI() {
	clock_t lastDraw=clock();
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
					SinglePlayer();
				} else
					uid ui_main_pre_id = 0;
			} else if(InRect(mx,my,uid rt_opt)) {
				if(msg.is_left() && msg.is_up()) {
					EnterOptions();
				} else
					uid ui_main_pre_id = 1;
			} else if(InRect(mx,my,uid rt_leave)) {
				if(msg.is_left() && msg.is_up()) {
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
				Leave();
			} else if(K(VK_SPACE) || K(VK_RETURN)) {
				SinglePlayer();
			} else if(K('O')) {
				EnterOptions();
			}
		}
	}
}
}darkVoxel;
////////////////////////////////////////////////////////////////////////////////////////
int main() {
	CreateFile(LOG_NAME,"===========================================");
	DebugLog("        DarkVoxel2 运行日志   ");
	uix = GetSystemWorkAreaWidth();
	uiy = GetSystemWorkAreaHeight() - GetSystemMetrics(SM_CYCAPTION);
	initgraph(uix, uiy, INIT_RENDERMANUAL);
	delay_fps(60);
	setbkcolor(BLACK);
	setcaption(strcat_cc("DarkVoxel ",CURRENT_VERSION));
	hwnd = getHWnd();
	setrendermode(RENDER_MANUAL);
	SetWindowPos(hwnd,NULL,0,0,0,0,SWP_NOSIZE|SWP_NOZORDER);	//左上贴着
	ShowWindow(hwnd, SW_MAXIMIZE);

	darkVoxel._GetTheDirs();//获取存档路径
	darkVoxel.Splash();
	if(darkVoxel.firstRun) {
		darkVoxel.Beginning();
	}
	darkVoxel.EnterMainUI();

	closegraph();
	return 0;
}
