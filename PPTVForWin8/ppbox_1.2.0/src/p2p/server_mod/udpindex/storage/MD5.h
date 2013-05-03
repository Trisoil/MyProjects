//
// MD5.h
//
// Copyright (c) Shareaza Development Team, 2002-2005.
// This file is part of SHAREAZA (www.shareaza.com)
//
// Shareaza is free software; you can redistribute it
// and/or modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2 of
// the License, or (at your option) any later version.
//
// Shareaza is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Shareaza; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//

#if !defined(AFX_MD5_H__0C3A876B_CD09_4415_A661_35167D882CFD__INCLUDED_)
#define AFX_MD5_H__0C3A876B_CD09_4415_A661_35167D882CFD__INCLUDED_

#pragma once

typedef union
{
	BYTE	n[16];
	BYTE	b[16];
	DWORD	w[4];
} SMD4, SMD5;

class CMD5
	:boost::noncopyable
{
// Construction
public:
	typedef boost::shared_ptr<CMD5> p;
	static CMD5::p Create(){ return p(new CMD5());};
	virtual ~CMD5();

// Attributes
public:
	DWORD	m_nCount[2];
	DWORD	m_nState[4];
	BYTE	m_nBuffer[64];

// Operations
public:
	virtual void	Reset();
	virtual void	Add(LPCVOID pData, size_t nLength);
	virtual void	Finish();
	void			GetHash(SMD5* pHash);
public:
	static tstring	HashToString(const SMD5* pHash, BOOL bURN = FALSE);
	static BOOL		HashFromString(LPCTSTR pszHash, SMD5* pMD5);
	static BOOL		HashFromURN(LPCTSTR pszHash, SMD5* pMD5);

// Implementation
protected:
	CMD5();
	//void			Transform(BYTE* pBlock);
	//virtual void	Encode(BYTE* pOutput, DWORD* pInput, DWORD nLength);
	//virtual void	Decode(DWORD* pOutput, BYTE* pInput, DWORD nLength);
};
static BYTE KEY_G_[] ={0x01,0x02,0x03,0x04}; 

class CHmacMD5
	:public CMD5
{
#define DEFAULT_KEY_LEN 64

// Construction
public:
	typedef boost::shared_ptr<CHmacMD5> p;
	static CHmacMD5::p Create(){ return p(new CHmacMD5());};
	virtual ~CHmacMD5();
protected:
	CHmacMD5(u_char *key =KEY_G_ ,size_t keylen = sizeof(KEY_G_));

private:
	BYTE    m_key[64];
	bool	b_reset_;

// Operations
public:
	virtual void	Reset();
	virtual void	Add(LPCVOID pData, size_t nLength);
	virtual void	Finish();
// Implementation
};
#endif // !defined(AFX_MD5_H__0C3A876B_CD09_4415_A661_35167D882CFD__INCLUDED_)
