// Do NOT change. Changes will be lost next time file is generated

#define R__DICTIONARY_FILENAME mainAnalysisSelectorDict
#define R__NO_DEPRECATION

/*******************************************************************/
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#define G__DICTIONARY
#include "RConfig.h"
#include "TClass.h"
#include "TDictAttributeMap.h"
#include "TInterpreter.h"
#include "TROOT.h"
#include "TBuffer.h"
#include "TMemberInspector.h"
#include "TInterpreter.h"
#include "TVirtualMutex.h"
#include "TError.h"

#ifndef G__ROOT
#define G__ROOT
#endif

#include "RtypesImp.h"
#include "TIsAProxy.h"
#include "TFileMergeInfo.h"
#include <algorithm>
#include "TCollectionProxyInfo.h"
/*******************************************************************/

#include "TDataMember.h"

// The generated code does not explicitly qualifies STL entities
namespace std {} using namespace std;

// Header files passed as explicit arguments
#include "mainAnalysisSelector.h"

// Header files passed via #pragma extra_include

namespace ROOT {
   static void *new_mainAnalysisSelector(void *p = 0);
   static void *newArray_mainAnalysisSelector(Long_t size, void *p);
   static void delete_mainAnalysisSelector(void *p);
   static void deleteArray_mainAnalysisSelector(void *p);
   static void destruct_mainAnalysisSelector(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::mainAnalysisSelector*)
   {
      ::mainAnalysisSelector *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TInstrumentedIsAProxy< ::mainAnalysisSelector >(0);
      static ::ROOT::TGenericClassInfo 
         instance("mainAnalysisSelector", ::mainAnalysisSelector::Class_Version(), "mainAnalysisSelector.h", 29,
                  typeid(::mainAnalysisSelector), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &::mainAnalysisSelector::Dictionary, isa_proxy, 4,
                  sizeof(::mainAnalysisSelector) );
      instance.SetNew(&new_mainAnalysisSelector);
      instance.SetNewArray(&newArray_mainAnalysisSelector);
      instance.SetDelete(&delete_mainAnalysisSelector);
      instance.SetDeleteArray(&deleteArray_mainAnalysisSelector);
      instance.SetDestructor(&destruct_mainAnalysisSelector);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::mainAnalysisSelector*)
   {
      return GenerateInitInstanceLocal((::mainAnalysisSelector*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const ::mainAnalysisSelector*)0x0); R__UseDummy(_R__UNIQUE_DICT_(Init));
} // end of namespace ROOT

//______________________________________________________________________________
atomic_TClass_ptr mainAnalysisSelector::fgIsA(0);  // static to hold class pointer

//______________________________________________________________________________
const char *mainAnalysisSelector::Class_Name()
{
   return "mainAnalysisSelector";
}

//______________________________________________________________________________
const char *mainAnalysisSelector::ImplFileName()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::mainAnalysisSelector*)0x0)->GetImplFileName();
}

//______________________________________________________________________________
int mainAnalysisSelector::ImplFileLine()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::mainAnalysisSelector*)0x0)->GetImplFileLine();
}

//______________________________________________________________________________
TClass *mainAnalysisSelector::Dictionary()
{
   fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::mainAnalysisSelector*)0x0)->GetClass();
   return fgIsA;
}

//______________________________________________________________________________
TClass *mainAnalysisSelector::Class()
{
   if (!fgIsA.load()) { R__LOCKGUARD(gInterpreterMutex); fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::mainAnalysisSelector*)0x0)->GetClass(); }
   return fgIsA;
}

//______________________________________________________________________________
void mainAnalysisSelector::Streamer(TBuffer &R__b)
{
   // Stream an object of class mainAnalysisSelector.

   if (R__b.IsReading()) {
      R__b.ReadClassBuffer(mainAnalysisSelector::Class(),this);
   } else {
      R__b.WriteClassBuffer(mainAnalysisSelector::Class(),this);
   }
}

namespace ROOT {
   // Wrappers around operator new
   static void *new_mainAnalysisSelector(void *p) {
      return  p ? new(p) ::mainAnalysisSelector : new ::mainAnalysisSelector;
   }
   static void *newArray_mainAnalysisSelector(Long_t nElements, void *p) {
      return p ? new(p) ::mainAnalysisSelector[nElements] : new ::mainAnalysisSelector[nElements];
   }
   // Wrapper around operator delete
   static void delete_mainAnalysisSelector(void *p) {
      delete ((::mainAnalysisSelector*)p);
   }
   static void deleteArray_mainAnalysisSelector(void *p) {
      delete [] ((::mainAnalysisSelector*)p);
   }
   static void destruct_mainAnalysisSelector(void *p) {
      typedef ::mainAnalysisSelector current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::mainAnalysisSelector

namespace ROOT {
   static TClass *vectorlEdoublegR_Dictionary();
   static void vectorlEdoublegR_TClassManip(TClass*);
   static void *new_vectorlEdoublegR(void *p = 0);
   static void *newArray_vectorlEdoublegR(Long_t size, void *p);
   static void delete_vectorlEdoublegR(void *p);
   static void deleteArray_vectorlEdoublegR(void *p);
   static void destruct_vectorlEdoublegR(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const vector<double>*)
   {
      vector<double> *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(vector<double>));
      static ::ROOT::TGenericClassInfo 
         instance("vector<double>", -2, "vector", 339,
                  typeid(vector<double>), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &vectorlEdoublegR_Dictionary, isa_proxy, 0,
                  sizeof(vector<double>) );
      instance.SetNew(&new_vectorlEdoublegR);
      instance.SetNewArray(&newArray_vectorlEdoublegR);
      instance.SetDelete(&delete_vectorlEdoublegR);
      instance.SetDeleteArray(&deleteArray_vectorlEdoublegR);
      instance.SetDestructor(&destruct_vectorlEdoublegR);
      instance.AdoptCollectionProxyInfo(TCollectionProxyInfo::Generate(TCollectionProxyInfo::Pushback< vector<double> >()));
      return &instance;
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const vector<double>*)0x0); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *vectorlEdoublegR_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const vector<double>*)0x0)->GetClass();
      vectorlEdoublegR_TClassManip(theClass);
   return theClass;
   }

   static void vectorlEdoublegR_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrappers around operator new
   static void *new_vectorlEdoublegR(void *p) {
      return  p ? ::new((::ROOT::Internal::TOperatorNewHelper*)p) vector<double> : new vector<double>;
   }
   static void *newArray_vectorlEdoublegR(Long_t nElements, void *p) {
      return p ? ::new((::ROOT::Internal::TOperatorNewHelper*)p) vector<double>[nElements] : new vector<double>[nElements];
   }
   // Wrapper around operator delete
   static void delete_vectorlEdoublegR(void *p) {
      delete ((vector<double>*)p);
   }
   static void deleteArray_vectorlEdoublegR(void *p) {
      delete [] ((vector<double>*)p);
   }
   static void destruct_vectorlEdoublegR(void *p) {
      typedef vector<double> current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class vector<double>

namespace ROOT {
   static TClass *maplEintcOTH1mUgR_Dictionary();
   static void maplEintcOTH1mUgR_TClassManip(TClass*);
   static void *new_maplEintcOTH1mUgR(void *p = 0);
   static void *newArray_maplEintcOTH1mUgR(Long_t size, void *p);
   static void delete_maplEintcOTH1mUgR(void *p);
   static void deleteArray_maplEintcOTH1mUgR(void *p);
   static void destruct_maplEintcOTH1mUgR(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const map<int,TH1*>*)
   {
      map<int,TH1*> *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(map<int,TH1*>));
      static ::ROOT::TGenericClassInfo 
         instance("map<int,TH1*>", -2, "map", 100,
                  typeid(map<int,TH1*>), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &maplEintcOTH1mUgR_Dictionary, isa_proxy, 0,
                  sizeof(map<int,TH1*>) );
      instance.SetNew(&new_maplEintcOTH1mUgR);
      instance.SetNewArray(&newArray_maplEintcOTH1mUgR);
      instance.SetDelete(&delete_maplEintcOTH1mUgR);
      instance.SetDeleteArray(&deleteArray_maplEintcOTH1mUgR);
      instance.SetDestructor(&destruct_maplEintcOTH1mUgR);
      instance.AdoptCollectionProxyInfo(TCollectionProxyInfo::Generate(TCollectionProxyInfo::MapInsert< map<int,TH1*> >()));
      return &instance;
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const map<int,TH1*>*)0x0); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *maplEintcOTH1mUgR_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const map<int,TH1*>*)0x0)->GetClass();
      maplEintcOTH1mUgR_TClassManip(theClass);
   return theClass;
   }

   static void maplEintcOTH1mUgR_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrappers around operator new
   static void *new_maplEintcOTH1mUgR(void *p) {
      return  p ? ::new((::ROOT::Internal::TOperatorNewHelper*)p) map<int,TH1*> : new map<int,TH1*>;
   }
   static void *newArray_maplEintcOTH1mUgR(Long_t nElements, void *p) {
      return p ? ::new((::ROOT::Internal::TOperatorNewHelper*)p) map<int,TH1*>[nElements] : new map<int,TH1*>[nElements];
   }
   // Wrapper around operator delete
   static void delete_maplEintcOTH1mUgR(void *p) {
      delete ((map<int,TH1*>*)p);
   }
   static void deleteArray_maplEintcOTH1mUgR(void *p) {
      delete [] ((map<int,TH1*>*)p);
   }
   static void destruct_maplEintcOTH1mUgR(void *p) {
      typedef map<int,TH1*> current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class map<int,TH1*>

namespace ROOT {
   static TClass *maplETStringcOmaplEintcOTH1mUgRsPgR_Dictionary();
   static void maplETStringcOmaplEintcOTH1mUgRsPgR_TClassManip(TClass*);
   static void *new_maplETStringcOmaplEintcOTH1mUgRsPgR(void *p = 0);
   static void *newArray_maplETStringcOmaplEintcOTH1mUgRsPgR(Long_t size, void *p);
   static void delete_maplETStringcOmaplEintcOTH1mUgRsPgR(void *p);
   static void deleteArray_maplETStringcOmaplEintcOTH1mUgRsPgR(void *p);
   static void destruct_maplETStringcOmaplEintcOTH1mUgRsPgR(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const map<TString,map<int,TH1*> >*)
   {
      map<TString,map<int,TH1*> > *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(map<TString,map<int,TH1*> >));
      static ::ROOT::TGenericClassInfo 
         instance("map<TString,map<int,TH1*> >", -2, "map", 100,
                  typeid(map<TString,map<int,TH1*> >), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &maplETStringcOmaplEintcOTH1mUgRsPgR_Dictionary, isa_proxy, 0,
                  sizeof(map<TString,map<int,TH1*> >) );
      instance.SetNew(&new_maplETStringcOmaplEintcOTH1mUgRsPgR);
      instance.SetNewArray(&newArray_maplETStringcOmaplEintcOTH1mUgRsPgR);
      instance.SetDelete(&delete_maplETStringcOmaplEintcOTH1mUgRsPgR);
      instance.SetDeleteArray(&deleteArray_maplETStringcOmaplEintcOTH1mUgRsPgR);
      instance.SetDestructor(&destruct_maplETStringcOmaplEintcOTH1mUgRsPgR);
      instance.AdoptCollectionProxyInfo(TCollectionProxyInfo::Generate(TCollectionProxyInfo::MapInsert< map<TString,map<int,TH1*> > >()));
      return &instance;
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const map<TString,map<int,TH1*> >*)0x0); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *maplETStringcOmaplEintcOTH1mUgRsPgR_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const map<TString,map<int,TH1*> >*)0x0)->GetClass();
      maplETStringcOmaplEintcOTH1mUgRsPgR_TClassManip(theClass);
   return theClass;
   }

   static void maplETStringcOmaplEintcOTH1mUgRsPgR_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrappers around operator new
   static void *new_maplETStringcOmaplEintcOTH1mUgRsPgR(void *p) {
      return  p ? ::new((::ROOT::Internal::TOperatorNewHelper*)p) map<TString,map<int,TH1*> > : new map<TString,map<int,TH1*> >;
   }
   static void *newArray_maplETStringcOmaplEintcOTH1mUgRsPgR(Long_t nElements, void *p) {
      return p ? ::new((::ROOT::Internal::TOperatorNewHelper*)p) map<TString,map<int,TH1*> >[nElements] : new map<TString,map<int,TH1*> >[nElements];
   }
   // Wrapper around operator delete
   static void delete_maplETStringcOmaplEintcOTH1mUgRsPgR(void *p) {
      delete ((map<TString,map<int,TH1*> >*)p);
   }
   static void deleteArray_maplETStringcOmaplEintcOTH1mUgRsPgR(void *p) {
      delete [] ((map<TString,map<int,TH1*> >*)p);
   }
   static void destruct_maplETStringcOmaplEintcOTH1mUgRsPgR(void *p) {
      typedef map<TString,map<int,TH1*> > current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class map<TString,map<int,TH1*> >

namespace ROOT {
   static TClass *maplETStringcOcommoncLcLTHInfomUgR_Dictionary();
   static void maplETStringcOcommoncLcLTHInfomUgR_TClassManip(TClass*);
   static void *new_maplETStringcOcommoncLcLTHInfomUgR(void *p = 0);
   static void *newArray_maplETStringcOcommoncLcLTHInfomUgR(Long_t size, void *p);
   static void delete_maplETStringcOcommoncLcLTHInfomUgR(void *p);
   static void deleteArray_maplETStringcOcommoncLcLTHInfomUgR(void *p);
   static void destruct_maplETStringcOcommoncLcLTHInfomUgR(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const map<TString,common::THInfo*>*)
   {
      map<TString,common::THInfo*> *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(map<TString,common::THInfo*>));
      static ::ROOT::TGenericClassInfo 
         instance("map<TString,common::THInfo*>", -2, "map", 100,
                  typeid(map<TString,common::THInfo*>), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &maplETStringcOcommoncLcLTHInfomUgR_Dictionary, isa_proxy, 0,
                  sizeof(map<TString,common::THInfo*>) );
      instance.SetNew(&new_maplETStringcOcommoncLcLTHInfomUgR);
      instance.SetNewArray(&newArray_maplETStringcOcommoncLcLTHInfomUgR);
      instance.SetDelete(&delete_maplETStringcOcommoncLcLTHInfomUgR);
      instance.SetDeleteArray(&deleteArray_maplETStringcOcommoncLcLTHInfomUgR);
      instance.SetDestructor(&destruct_maplETStringcOcommoncLcLTHInfomUgR);
      instance.AdoptCollectionProxyInfo(TCollectionProxyInfo::Generate(TCollectionProxyInfo::MapInsert< map<TString,common::THInfo*> >()));
      return &instance;
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const map<TString,common::THInfo*>*)0x0); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *maplETStringcOcommoncLcLTHInfomUgR_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const map<TString,common::THInfo*>*)0x0)->GetClass();
      maplETStringcOcommoncLcLTHInfomUgR_TClassManip(theClass);
   return theClass;
   }

   static void maplETStringcOcommoncLcLTHInfomUgR_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrappers around operator new
   static void *new_maplETStringcOcommoncLcLTHInfomUgR(void *p) {
      return  p ? ::new((::ROOT::Internal::TOperatorNewHelper*)p) map<TString,common::THInfo*> : new map<TString,common::THInfo*>;
   }
   static void *newArray_maplETStringcOcommoncLcLTHInfomUgR(Long_t nElements, void *p) {
      return p ? ::new((::ROOT::Internal::TOperatorNewHelper*)p) map<TString,common::THInfo*>[nElements] : new map<TString,common::THInfo*>[nElements];
   }
   // Wrapper around operator delete
   static void delete_maplETStringcOcommoncLcLTHInfomUgR(void *p) {
      delete ((map<TString,common::THInfo*>*)p);
   }
   static void deleteArray_maplETStringcOcommoncLcLTHInfomUgR(void *p) {
      delete [] ((map<TString,common::THInfo*>*)p);
   }
   static void destruct_maplETStringcOcommoncLcLTHInfomUgR(void *p) {
      typedef map<TString,common::THInfo*> current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class map<TString,common::THInfo*>

namespace {
  void TriggerDictionaryInitialization_mainAnalysisSelectorDict_Impl() {
    static const char* headers[] = {
"mainAnalysisSelector.h",
0
    };
    static const char* includePaths[] = {
"/cvmfs/sft.cern.ch/lcg/releases/ROOT/6.18.00-7e814/x86_64-slc6-gcc8-opt/include",
"/uscms_data/d3/as2872/iDM/AndreAnalysis/CMSSW_10_2_18/src/iDMAnalysis/skimmer/macros/TSelectors/",
0
    };
    static const char* fwdDeclCode = R"DICTFWDDCLS(
#line 1 "mainAnalysisSelectorDict dictionary forward declarations' payload"
#pragma clang diagnostic ignored "-Wkeyword-compat"
#pragma clang diagnostic ignored "-Wignored-attributes"
#pragma clang diagnostic ignored "-Wreturn-type-c-linkage"
extern int __Cling_Autoloading_Map;
class __attribute__((annotate(R"ATTRDUMP(file_name@@@mainAnalysisSelector.h)ATTRDUMP"))) __attribute__((annotate(R"ATTRDUMP(pattern@@@*)ATTRDUMP"))) __attribute__((annotate("$clingAutoload$mainAnalysisSelector.h")))  mainAnalysisSelector;
)DICTFWDDCLS";
    static const char* payloadCode = R"DICTPAYLOAD(
#line 1 "mainAnalysisSelectorDict dictionary payload"


#define _BACKWARD_BACKWARD_WARNING_H
// Inline headers
#include "mainAnalysisSelector.h"

#undef  _BACKWARD_BACKWARD_WARNING_H
)DICTPAYLOAD";
    static const char* classesHeaders[]={
"", payloadCode, "@",
"mainAnalysisSelector", payloadCode, "@",
"mainAnalysisSelector::fgIsA", payloadCode, "@",
nullptr};

    static bool isInitialized = false;
    if (!isInitialized) {
      TROOT::RegisterModule("mainAnalysisSelectorDict",
        headers, includePaths, payloadCode, fwdDeclCode,
        TriggerDictionaryInitialization_mainAnalysisSelectorDict_Impl, {}, classesHeaders, /*has no C++ module*/false);
      isInitialized = true;
    }
  }
  static struct DictInit {
    DictInit() {
      TriggerDictionaryInitialization_mainAnalysisSelectorDict_Impl();
    }
  } __TheDictionaryInitializer;
}
void TriggerDictionaryInitialization_mainAnalysisSelectorDict() {
  TriggerDictionaryInitialization_mainAnalysisSelectorDict_Impl();
}
