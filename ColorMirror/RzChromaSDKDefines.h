//! \file RzChromaSDKDefines.h
//! \brief Defines.
//!
//! Definitions of global and static variables.

#ifndef _RZSCHROMADKDEFINES_H_
#define _RZSCHROMADKDEFINES_H_

#pragma once

#ifndef GUID_DEFINED
#include <Guiddef.h>
#endif

namespace ChromaSDK
{

//! Razer Blackwidow Chroma device Id.
// {2EA1BB63-CA28-428D-9F06-196B88330BBB}
static const GUID BLACKWIDOW_CHROMA = 
{ 0x2ea1bb63, 0xca28, 0x428d, { 0x9f, 0x06, 0x19, 0x6b, 0x88, 0x33, 0x0b, 0xbb } };

// {AEC50D91-B1F1-452F-8E16-7B73F376FDF3}
static const GUID DEATHADDER_CHROMA = 
{ 0xaec50d91, 0xb1f1, 0x452f, { 0x8e, 0x16, 0x7b, 0x73, 0xf3, 0x76, 0xfd, 0xf3 } };

// {29A02ACB-B882-4EE3-8045-AE7A8A210092}
static const GUID NAGAEPIC_CHROMA = 
{ 0x29a02acb, 0xb882, 0x4ee3, { 0x80, 0x45, 0xae, 0x7a, 0x8a, 0x21, 0x0, 0x92 } };

// {CD1E09A5-D5E6-4A6C-A93B-E6D9BF1D2092}
static const GUID KRAKEN71_CHROMA = 
{ 0xcd1e09a5, 0xd5e6, 0x4a6c, { 0xa9, 0x3b, 0xe6, 0xd9, 0xbf, 0x1d, 0x20, 0x92 } };

// {184E4849-D6E9-413A-A753-15747B6389A2}
static const GUID ALL_CHROMA_KEYBOARDS = 
{ 0x184e4849, 0xd6e9, 0x413a, { 0xa7, 0x53, 0x15, 0x74, 0x7b, 0x63, 0x89, 0xa2 } };

// {E554EF43-3BD3-4357-88E5-B2A8D5C5AE7B}
static const GUID ALL_CHROMA_MICE = 
{ 0xe554ef43, 0x3bd3, 0x4357, { 0x88, 0xe5, 0xb2, 0xa8, 0xd5, 0xc5, 0xae, 0x7b } };

// {58AAE042-9A88-432F-8A3B-4BC8336B7468}
static const GUID ALL_CHROMA_HEADSETS = 
{ 0x58aae042, 0x9a88, 0x432f, { 0x8a, 0x3b, 0x4b, 0xc8, 0x33, 0x6b, 0x74, 0x68 } };

// {5BF2A39C-621B-4B04-BA4D-DFFC107C214D}
static const GUID CHROMA_DEVICES = 
{ 0x5bf2a39c, 0x621b, 0x4b04, { 0xba, 0x4d, 0xdf, 0xfc, 0x10, 0x7c, 0x21, 0x4d } };

}


#endif
