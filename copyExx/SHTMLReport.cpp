#include "stdafx.h"
#include "sreport.h"
#include "SHTMLReport.h"
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CSHTMLReport::CSHTMLReport()
{
	m_pHtmlDoc2=NULL;
	m_iHeadLines=1;
}
CSHTMLReport::~CSHTMLReport()
{
}
//***********************************************
// 指定HTML文档接口
//**********************************************
void CSHTMLReport::SetHtmlDocPtr(IHTMLDocument2 *pDoc)
{
	m_pHtmlDoc2=pDoc;
}
//***********************************************
// 指定操作的表的名称,表名称在HTML模板中指定
//*********************************************
void CSHTMLReport::SetTableName(CString name)
{
	m_strTableName=name;
}
//**************************************************
// 获取HTML表格COM接口，内部接口
//**************************************************
IHTMLTable * CSHTMLReport::GetTableDispatch()
{
	ASSERT(m_pHtmlDoc2);
	IHTMLElementCollection *all;
	m_pHtmlDoc2->get_all(&all);
	IDispatch *distable;
	all->item(COleVariant(m_strTableName),COleVariant(short(0)),&distable);
	IHTMLTable *pITable=NULL;
	HRESULT hr=distable->QueryInterface(IID_IHTMLTable, (void**)&pITable);//get table dispatch
	ASSERT(hr==S_OK);
	return pITable;
}
//*******************************************************
// 在指定位置插入一行，index==-1表示append
//*******************************************************
BOOL CSHTMLReport::InsertRow(int index)
{
	IHTMLTable *pITable=GetTableDispatch();
	IDispatch *disrow;
	HRESULT hr=pITable->insertRow(index,&disrow);//insert a row at 1 position
	if(hr!=S_OK) return FALSE;
	IHTMLTableRow *pIRow;
	hr=disrow->QueryInterface(IID_IHTMLTableRow, (void**)&pIRow);
	ASSERT(hr==S_OK);
	long cols;
	hr=pITable->get_cols(&cols);
	ASSERT(hr==S_OK&&cols!=0);
	IDispatch *discell;
	IHTMLElement *cell;
	CString str= "&nbsp;";
	BSTR bsStr = str.AllocSysString();
	for(int i=0;i<cols;i++)
	{
		pIRow->insertCell(i,&discell);
		discell->QueryInterface(IID_IHTMLElement,(void **)&cell);
		cell->put_innerHTML(bsStr);
		cell->Release();
		discell->Release();
	}
	SysFreeString(bsStr);
	pIRow->Release();
	if(!m_strIndexFormat.IsEmpty())
	{
		IHTMLElementCollection *irows;
		hr=pITable->get_rows(&irows);
		ASSERT(hr==S_OK);
		long rows;
		hr=irows->get_length(&rows);
		ASSERT(hr==S_OK);
		CString strHead="";
		if(index==-1) index=rows-1;
		for(long i=index;i<rows;i++)
		{
			strHead.Format(m_strIndexFormat,i);
			BSTR bsStrHead=strHead.AllocSysString();
			hr=irows->item(COleVariant(long(i)),COleVariant(long(0)),&disrow);
			ASSERT(hr==S_OK);
			//get row interface
			hr=disrow->QueryInterface(IID_IHTMLTableRow, (void**)&pIRow);
			ASSERT(hr==S_OK);
			IHTMLElementCollection *icells;
			hr=pIRow->get_cells(&icells);
			ASSERT(hr==S_OK);
			hr=icells->item(COleVariant(long(0)),COleVariant(long(0)),&discell);
			ASSERT(hr==S_OK);
			discell->QueryInterface(IID_IHTMLElement,(void **)&cell);
			ASSERT(hr==S_OK);
			cell->put_innerText(bsStrHead);
			SysFreeString(bsStrHead);
			cell->Release();
			pIRow->Release();
			disrow->Release();
		}
		irows->Release();
	}
	pIRow->Release();
	disrow->Release();
	pITable->Release();
	return TRUE;
}
//***************************************************************
// 修改单元格的内容：可以使用html语法
//***************************************************************
BOOL CSHTMLReport::SetItemHTML(int iRow, int iCol, CString html)
{
	IHTMLTable *pITable=GetTableDispatch();
	//get row collect
	IHTMLElementCollection *pIRows;
	HRESULT hr=pITable->get_rows(&pIRows);
	ASSERT(hr==S_OK);
	//get specisfied row
	IDispatch *disrow;
	hr=pIRows->item(COleVariant(long(iRow)),COleVariant(short(0)),&disrow);//iRow row
	if(hr!=S_OK||disrow==NULL) return FALSE;
	IHTMLTableRow *pIRow;
	hr=disrow->QueryInterface(IID_IHTMLTableRow, (void**)&pIRow);
	ASSERT(hr==S_OK);
	//get cell collect
	IHTMLElementCollection *rowcells;
	pIRow->get_cells(&rowcells);
	//get cell
	IDispatch *discell;
	IHTMLElement *cell;
	hr=rowcells->item(COleVariant(long(iCol)),COleVariant(short(0)),&discell);//iCol col
	if(hr!=S_OK||discell==NULL) return FALSE;
	hr=discell->QueryInterface(IID_IHTMLElement,(void **)&cell);
	ASSERT(hr==S_OK);
	BSTR bsStr = html.AllocSysString();
	cell->put_innerHTML(bsStr);
	SysFreeString(bsStr);
	cell->Release();
	pIRow->Release();
	pITable->Release();
	return TRUE;
}
//************************************************
// 删除指定行
//************************************************
BOOL CSHTMLReport::DeleteRow(int index)
{
	IHTMLTable *pITable=GetTableDispatch();
	HRESULT hr=pITable->deleteRow(index);
	pITable->Release();
	return hr==S_OK;
}
//************************************************
// 将指定行数据相同的单元格合并
//************************************************
BOOL CSHTMLReport::MergeRow(int iRow)
{
	IHTMLTable *pITable=GetTableDispatch();
	//get row collect
	IHTMLElementCollection *pIRows;
	HRESULT hr=pITable->get_rows(&pIRows);
	ASSERT(hr==S_OK);
	//get specisfied row
	IDispatch *disrow;
	hr=pIRows->item(COleVariant(long(iRow)),COleVariant(short(0)),&disrow);//iRow row
	if(hr!=S_OK||disrow==NULL) return FALSE;
	IHTMLTableRow *pIRow;
	hr=disrow->QueryInterface(IID_IHTMLTableRow, (void**)&pIRow);
	ASSERT(hr==S_OK);
	//get cell collect
	IHTMLElementCollection *rowcells;
	pIRow->get_cells(&rowcells);
	long cells;
	hr=rowcells->get_length(&cells);
	IDispatch *discell;
	IHTMLElement *icellElement;
	long begin=-1,end=-1;
	CString beginStr;
	CString cellStr;
	long beginRowSpan=0,rowSpan;
	for(long i=m_strIndexFormat.IsEmpty()?0:1;i<cells;i++)
	{
		//get cell
		hr=rowcells->item(COleVariant(long(i)),COleVariant(short(0)),&discell);
		ASSERT(hr==S_OK&&discell!=NULL);
		hr=discell->QueryInterface(IID_IHTMLElement,(void **)&icellElement);
		ASSERT(hr==S_OK&&icellElement!=NULL);

		cellStr=GetElementText(icellElement,inner_text);
		IHTMLTableCell *icell;
		hr=discell->QueryInterface(IID_IHTMLTableCell,(void **)&icell);
		ASSERT(hr==S_OK&&icell!=NULL);
		icell->get_rowSpan(&rowSpan);
		if(cellStr!=beginStr||rowSpan!=beginRowSpan)
		{
			if(end-begin+1>1&&begin!=-1)
			{
				ASSERT(beginRowSpan!=0);
				MergeRowPrivate(pIRow,begin,end);
				cells-=end-begin;
				i-=end-begin;
			}
			begin=end=i;
			beginStr=cellStr;
			beginRowSpan=rowSpan;
		}else
			end=i;
		icell->Release();
		icellElement->Release();
	}
	if(begin!=end)
		MergeRowPrivate(pIRow,begin,end);
	pIRow->Release();
	pITable->Release();
	return TRUE;
}
//************************************************
// 将指定列数据相同的单元格合并
//************************************************
BOOL CSHTMLReport::MergeCol(int iCol)
{
	IHTMLTable *pITable=GetTableDispatch();
	//get row collect
	IHTMLElementCollection *pIRows;
	HRESULT hr=pITable->get_rows(&pIRows);
	ASSERT(hr==S_OK);
	CSArray<COLCELLINFO> colArray;
	if(!MakeColDispatchCollection(pIRows,iCol,colArray)) return FALSE;
	int colCells=colArray.GetSize();
	COLCELLINFO cci;
	CString beginStr,cellStr;
	long beginColSpan=0,colSpan;
	int begin=-1,end;
	for(int i=0;i<colCells;i++)
	{
		cci=colArray.GetAt(i);
		if(cci.pDisCell==NULL)
			continue;
		IHTMLElement *icellElement;
		hr=cci.pDisCell->QueryInterface(IID_IHTMLElement,(void **)&icellElement);
		ASSERT(hr==S_OK&&icellElement!=NULL);
		cellStr=GetElementText(icellElement,inner_text);
		IHTMLTableCell *icell;
		hr=cci.pDisCell->QueryInterface(IID_IHTMLTableCell,(void **)&icell);
		ASSERT(hr==S_OK&&icell!=NULL);
		hr=icell->get_colSpan(&colSpan);
		if(cellStr!=beginStr||colSpan!=beginColSpan)
		{
			if(begin!=-1&&end-begin+1>1)
			{
				ASSERT(beginColSpan!=0);
				MergeColPrivate(pIRows,colArray,begin,end);
			}
			begin=end=i;
			beginStr=cellStr;
			beginColSpan=colSpan;
		}else
			end=i;
		icell->Release();
		icellElement->Release();
	}
	if(begin!=end)
		MergeColPrivate(pIRows,colArray,begin,end);
	pITable->Release();
	return TRUE;
}
//***********************************************
// 设置索引列的显示格式：遵循sprintf函数的规则
//***********************************************
void CSHTMLReport::SetIndexFormat(CString strIndexFormat)
{
	m_strIndexFormat=strIndexFormat;
}
//*************************************************
// 获取元素的字符
//*************************************************
CString CSHTMLReport::GetElementText(IHTMLElement *pElement, int type)
{
	CString str= "";
	BSTR bsStr = str.AllocSysString();
	switch(type)
	{
	case inner_text:
		pElement->get_innerText(&bsStr);
		break;
	case inner_html:
		pElement->get_innerHTML(&bsStr);
		break;
	case outer_text:
		pElement->get_outerText(&bsStr);
		break;
	case outer_html:
		pElement->get_outerHTML(&bsStr);
		break;
	default:
		ASSERT(0);
		break;
	}
	_bstr_t bstStr;
	bstStr = bsStr;
	str.Format("%s",(LPCTSTR)bstStr);
	SysFreeString(bsStr);
	return str;
}
//********************************************************
// 设置元素字符
//*******************************************************
BOOL CSHTMLReport::SetElementText(IHTMLElement *pElement, int type,CString str)
{
	BSTR bsStr = str.AllocSysString();
	HRESULT hr;
	switch(type)
	{
	case inner_text:
		hr=pElement->put_innerText(bsStr);
		break;
	case inner_html:
		hr=pElement->put_innerHTML(bsStr);
		break;
	case outer_text:
		hr=pElement->put_outerText(bsStr);
		break;
	case outer_html:
		hr=pElement->put_outerHTML(bsStr);
		break;
	default:
		ASSERT(0);
		break;
	}
	SysFreeString(bsStr);
	return (hr==S_OK);
}
//******************************************************************
// 在指定的行中从指定的起始及终止位置进行合并：内部函数
//******************************************************************
void CSHTMLReport::MergeRowPrivate(IHTMLTableRow *pIRow, long begincol, long endcol)
{
	long colSpan=0;
	IHTMLElementCollection *picells;
	HRESULT hr=pIRow->get_cells(&picells);
	for(long i=endcol;i>=begincol;i--)
	{
		IDispatch *discell;
		hr=picells->item(COleVariant(i),COleVariant(short(0)),&discell);
		ASSERT(hr==S_OK&&discell!=NULL);
		IHTMLTableCell *picell;
		hr=discell->QueryInterface(IID_IHTMLTableCell,(void **)&picell);
		ASSERT(hr==S_OK&&picell!=NULL);
		long tmpColSpan;
		picell->get_colSpan(&tmpColSpan);
		colSpan+=tmpColSpan;
		if(i==begincol)
			picell->put_colSpan(colSpan);
		else
			pIRow->deleteCell(i);
		picell->Release();
	}
}
//******************************************************************
// 在指定的行集中对指定列从指定的起始及终止位置进行合并：内部函数
//******************************************************************
void CSHTMLReport::MergeColPrivate(IHTMLElementCollection *pIRowArray,CSArray<COLCELLINFO> &colArray,long beginrow, long endrow)
{
	IHTMLTableCell *ibegincell=NULL;
	long rowSpan=0;
	for(int i=beginrow;i<=endrow;i++)
	{
		IHTMLTableCell *icell;
		COLCELLINFO cci=colArray.GetAt(i);
		ASSERT(cci.iIndexInRow!=-1);
		IDispatch *disrow;
		HRESULT hr=pIRowArray->item(COleVariant(cci.iRow),COleVariant(short(0)),&disrow);//iRow row
		if(hr!=S_OK||disrow==NULL) return ;
		IHTMLTableRow *pIRow;
		hr=disrow->QueryInterface(IID_IHTMLTableRow, (void**)&pIRow);
		ASSERT(hr==S_OK);
		//get cell collection
		IHTMLElementCollection *rowcells;
		pIRow->get_cells(&rowcells);
		//get cell element
		IDispatch *discell;
		hr=rowcells->item(COleVariant(cci.iIndexInRow),COleVariant(short(0)),&discell);
		ASSERT(hr==S_OK&&discell!=NULL);

		hr=discell->QueryInterface(IID_IHTMLTableCell,(void **)&icell);
		ASSERT(hr==S_OK&&icell!=NULL);
		long tmpRowSpan;
		hr=icell->get_rowSpan(&tmpRowSpan);
		ASSERT(hr==S_OK);
		rowSpan+=tmpRowSpan;
		if(i==beginrow)
		{
			ibegincell=icell;
		}else{
			icell->Release(); 
			if(cci.iIndexInRow!=-1)
				pIRow->deleteCell(cci.iIndexInRow);
		}
		pIRow->Release();
	}
	ibegincell->put_rowSpan(rowSpan);
	ibegincell->Release();
}
//******************************************************************
// 构造指定列的集合：内部函数
//******************************************************************
BOOL CSHTMLReport::MakeColDispatchCollection(
	IHTMLElementCollection *pIRowArray/*in*/,
	long iCol/*in*/,
	CSArray<COLCELLINFO> &colArray/*out*/)
{
	long rows;
	HRESULT hr=pIRowArray->get_length(&rows);
	ASSERT(hr==S_OK);
	IHTMLElementCollection **rowcellsarray =new IHTMLElementCollection *[rows];
	//get cell collect array
	for(int j=0;j<rows;j++)
	{
		//get specisfied row
		IDispatch *disrow;
		hr=pIRowArray->item(COleVariant(long(j)),COleVariant(short(0)),&disrow);//iRow row
		if(hr!=S_OK||disrow==NULL) return FALSE;
		IHTMLTableRow *pIRow;
		hr=disrow->QueryInterface(IID_IHTMLTableRow, (void**)&pIRow);
		ASSERT(hr==S_OK);
		//get cell collection
		pIRow->get_cells(&rowcellsarray[j]);
		pIRow->Release();
	}
	struct colindexinfo{
		long index; //在行中的索引
		long iCol; //在列中的索引
		long colSpan;//列跨越
	};
	struct colindexinfo *ciis=new struct colindexinfo[rows];
	memset(ciis,0,rows*sizeof(struct colindexinfo));
	for(j=1;j<=iCol;j++)//追踪到第iCol列:初始化的数据即为第一列的数据，不需要计算
	{
		for(int k=0;k<rows;k++)
		{
			if(ciis[k].iCol+ciis[k].colSpan>j) continue;
			IDispatch *discell;
			hr=rowcellsarray[k]->item(COleVariant(ciis[k].index),COleVariant(short(0)),&discell);
			ASSERT(hr==S_OK&&discell!=NULL);
			IHTMLTableCell *icell;
			hr=discell->QueryInterface(IID_IHTMLTableCell,(void **)&icell);
			ASSERT(hr==S_OK&&icell!=NULL);
			long colSpan,rowSpan;
			hr=icell->get_colSpan(&colSpan);
			ASSERT(hr==S_OK);
			hr=icell->get_rowSpan(&rowSpan);
			icell->Release();
			ciis[k].index++;
			ciis[k].iCol+=colSpan;
			ciis[k].colSpan=colSpan-1;
			for(int kk=k+1;kk<k+rowSpan;kk++)
			{
				if(ciis[kk].iCol+ciis[kk].colSpan>j){
					delete []ciis;
					delete []rowcellsarray;
					return FALSE;
				}
				ciis[kk].iCol++;
				ciis[kk].colSpan=0;
			}
			k+=rowSpan-1;
		}
	}
	for(int i=0;i<rows;i++)
	{
		if(ciis[i].colSpan){
			COLCELLINFO cci={i,NULL,-1};
			colArray.Add(cci);
			continue;
		}
		IDispatch *discell;
		hr=rowcellsarray[i]->item(COleVariant(ciis[i].index),COleVariant(short(0)),&discell);
		ASSERT(hr==S_OK&&discell!=NULL);
		IHTMLTableCell *icell;
		hr=discell->QueryInterface(IID_IHTMLTableCell,(void **)&icell);
		ASSERT(hr==S_OK&&icell!=NULL);
		long rowSpan;
		hr=icell->get_rowSpan(&rowSpan);
		icell->Release();
		COLCELLINFO cci;
		cci.iRow=i;
		cci.iIndexInRow=ciis[i].index;
		cci.pDisCell=discell;
		colArray.Add(cci);
		i+=rowSpan-1;
	}
	delete []ciis;
	delete []rowcellsarray;
	return TRUE;
}
