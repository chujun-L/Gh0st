/***=========================================================================
====                                                                     ====
====                          D C U t i l i t y                          ====
====                                                                     ====
=============================================================================
====                                                                     ====
====    File name           :  TrueColorToolBar.h                        ====
====    Project name        :  Tester                                    ====
====    Project number      :  ---                                       ====
====    Creation date       :  13/1/2003                                 ====
====    Author(s)           :  Dany Cantin                               ====
====                                                                     ====
====                  Copyright ?DCUtility  2003                        ====
====                                                                     ====
=============================================================================
===========================================================================*/


#pragma once

#include <afxtempl.h>


// CTrueColorToolBar

class CTrueColorToolBar : public CToolBar
{
// Construction
public:
	CTrueColorToolBar();
	virtual ~CTrueColorToolBar();

	// Operations
	BOOL LoadTrueColorToolBar(int nBtnWidth, UINT uToolBar, 
							UINT uToolBarHot = 0, UINT uToolBarDisabled = 0);
	void AddDropDownButton(CWnd *pParent, UINT uButtonID, UINT uMenuID);

// Attributes
private:
	BOOL m_bDropDown;

	struct stDropDownInfo {
	public:
		UINT  uButtonID;
		UINT  uMenuID;
		CWnd* pParent;
	};
	
	CArray <stDropDownInfo, stDropDownInfo&> m_lstDropDownButton;
	
	BOOL SetTrueColorToolBar(UINT uToolBarType, UINT uToolBar, int nBtnWidth);

	// Generated message map functions
protected:
	afx_msg void OnToolbarDropDown(NMHDR * pnmh, LRESULT* plRes);
	DECLARE_MESSAGE_MAP()
};
