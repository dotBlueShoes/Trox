#pragma once
#include "Framework.hpp"

#ifndef STRICT
#define STRICT
#endif

#include <windows.h>
#include <stdio.h>
#include <shlobj.h>
#include <propkey.h>
#include <propvarutil.h>
#include <atlbase.h>
#include <atlalloc.h>



namespace MP3 {
	
	/// TO Free allocated memory with PropVariantToStringAlloc->CoTaskMemAlloc
	template<typename T>
	void CoTaskMemFreeArray(T **prgElem, ULONG cElem) {
		for (ULONG i = 0; i < cElem; i++) {
			CoTaskMemFree(prgElem[i]);
		}
		CoTaskMemFree(prgElem);
	}
	
	/// Template functions to Read property.
	template<typename Callback>
	HRESULT GetPropertyAsLambda (
		IPropertyStore *pps, 
		REFPROPERTYKEY key,
		Callback callback
	){
		PROPVARIANT pvar;
		HRESULT result = pps->GetValue(key, &pvar);
		
		if (result == S_OK || result == INPLACE_S_TRUNCATED) {
			result = callback(pvar);
			PropVariantClear(&pvar);
		} else {
			debug::LogError("Could not find specified property!");
		}
		
		return result;
	}
	
	/// Template functions to Write property.
	template<typename Callback>
	HRESULT SetPropertyAsLambda (
		IPropertyStore *pps, 
		REFPROPERTYKEY key,
		Callback callback
	) {
		PROPVARIANT pvar;
		HRESULT result = callback(&pvar);
		
		if (result == S_OK || result == INPLACE_S_TRUNCATED)  {
			result = pps->SetValue(key, pvar);
			PropVariantClear(&pvar);
		} else {
			debug::LogError("Could not find specified property!");
		}
		
		return result;
	}
	
	
	HRESULT GetPropertyAsString(
		IPropertyStore *pps, 
		REFPROPERTYKEY key, 
		PWSTR *ppszValue
	) {
		*ppszValue = nullptr;
		return GetPropertyAsLambda(pps, key, [=](REFPROPVARIANT pvar) {
			
			if constexpr (DEBUG) {
				if (pvar.vt == VT_EMPTY) debug::LogError("Property is empty unable to get string value!");
			}
			
			return PropVariantToStringAlloc(pvar, ppszValue);
		});
	}
	
	HRESULT GetPropertyAsStringVector(
		IPropertyStore *pps,
		REFPROPERTYKEY key,
		PWSTR **pprgsz,
		ULONG *pcElem
	) {
		*pprgsz = nullptr;
		*pcElem = 0;
		return GetPropertyAsLambda(pps, key, [=](REFPROPVARIANT pvar) {
			
			if constexpr (DEBUG) {
				if (pvar.vt == VT_EMPTY) debug::LogError("Property is empty unable to get string value!");
			}
			
			return PropVariantToStringVectorAlloc(pvar, pprgsz, pcElem);
		});
	}
	
	HRESULT SetPropertyAsString(
		IPropertyStore *pps, 
		REFPROPERTYKEY key, 
		PCWSTR pszValue
	) {
		return SetPropertyAsLambda(pps, key, [=](PROPVARIANT *ppvar) {
			return InitPropVariantFromString(pszValue, ppvar);
		});
	}
	
	HRESULT SetPropertyAsStringVector (
		IPropertyStore *pps, 
		REFPROPERTYKEY key,
		PCWSTR *prgsz, 
		ULONG cElems
	) {
		return SetPropertyAsLambda(pps, key, [=](PROPVARIANT *ppvar) {
			return InitPropVariantFromStringVector(prgsz, cElems, ppvar);
		});
	}
	
}