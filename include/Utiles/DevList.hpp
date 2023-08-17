#pragma once

extern "C"{
#include <Windows.h>
#include<iostream>
#include"strmif.h"
#include<initguid.h>
#include<vector>
#include<string>
#include<dshow.h>
}

#pragma comment(lib, "setupapi.lib")
#pragma comment(lib, "Ole32.lib")
#pragma comment(lib, "OleAut32.lib")
#pragma comment(lib, "strmiids.lib")

using namespace std;

namespace utiles {

class DevList{
public:
    DevList(){

    }
    ~DevList(){

    }
    int listVideoInputDevices(vector<string>& list)
    {
        return listDevices(CLSID_VideoInputDeviceCategory,list);
    }

    int listAudioInputDevices(vector<string>& list)
    {
        return listDevices(CLSID_AudioInputDeviceCategory,list);
    }

    int listDevices(/* [in] */ REFCLSID clsidDeviceClass,vector<string>& list)
    {
        ICreateDevEnum *pDevEnum = NULL;
        IEnumMoniker *pEnum = NULL;
        int deviceCounter = 0;
        CoInitialize(NULL);
        HRESULT hr = CoCreateInstance(
                    CLSID_SystemDeviceEnum,
                    NULL,
                    CLSCTX_INPROC_SERVER,
                    IID_ICreateDevEnum,
                    reinterpret_cast<void**>(&pDevEnum)
                    );
        char sName[1024] = {0};

        if (SUCCEEDED(hr))
        {
            hr = pDevEnum->CreateClassEnumerator(clsidDeviceClass,&pEnum, 0);
            if (hr == S_OK){

                IMoniker *pMoniker = NULL;
                while (pEnum->Next(1, &pMoniker, NULL) == S_OK)
                {
                    IPropertyBag *pPropBag;
                    hr = pMoniker->BindToStorage(0, 0, IID_IPropertyBag,(void**)(&pPropBag));

                    if (FAILED(hr)) {
                        pMoniker->Release();
                        continue; // Skip this one, maybe the next one will work.
                    }

                    VARIANT varName;
                    VariantInit(&varName);
                    hr = pPropBag->Read(L"Description", &varName, 0);
                    if (FAILED(hr))
                    {
                        hr = pPropBag->Read(L"FriendlyName", &varName, 0);
                    }

                    if (SUCCEEDED(hr))
                    {
                        hr = pPropBag->Read(L"FriendlyName", &varName, 0);
                        WideCharToMultiByte(CP_ACP, 0, varName.bstrVal, -1, sName, sizeof(sName)-1, "", NULL);
                        list.push_back(sName);
                        SysFreeString(varName.bstrVal);
                    }

                    pPropBag->Release();
                    pPropBag = NULL;
                    pMoniker->Release();
                    pMoniker = NULL;
                    deviceCounter++;
                }

                pDevEnum->Release();
                pDevEnum = NULL;
                pEnum->Release();
                pEnum = NULL;
            }
        }
        return deviceCounter;
    }
};


}
