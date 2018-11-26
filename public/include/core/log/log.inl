
namespace en
{
   namespace log
   {

//#if defined(EN_PLATFORM_WINDOWS)
//forceinline NLogContext* NLogContext::getPointer(void)
//{
// return &(NLogContext::getInstance()); 
//}
//
//forceinline void  NLogContext::init(void)
//{
// std::string logfilename;
// Config.get("en.core.log.file", logfilename);
//
// m_output = StorageContext.openForOverwriting(logfilename);
// if (m_output)
//    {
//    m_initialized = true;
//    *m_output << "Initialized context: Log\n";
//    }
//}
//
//forceinline void  NLogContext::destination(uint8 dst)
//{
// m_tofile = false;
// if (dst == File)
//    {
//    m_tofile = true;
//
//    if (!m_initialized)
//       init();
//    }
//}
//
//forceinline uint8 NLogContext::destination(void)
//{
// return m_tofile ? File : Console;
//}
//
//forceinline void NLogContext::on(void)
//{
// m_loging = true;
//
// if (m_tofile && !m_initialized)
//    init();
//}
//
//forceinline void NLogContext::off(void)
//{
// m_loging = false;
//}
//
//template <typename T>
//NLogContext& NLogContext::operator << (const T& content)
//{
// if (m_loging)
//    {
//#if defined(EN_PLATFORM_BLACKBERRY) || defined(EN_PLATFORM_WINDOWS)
//    if (m_tofile && m_initialized)
//       *m_output << content;
//    else
//       cout << content;
//#endif
//    }
// return *this;
//}
//#endif




   //template <typename T>
   //Interface& Interface::operator << (const T& content)
   //{
   // if (m_loging)
   //    {
   //    if (m_tofile && m_initialized)
   //       *m_output << content;
   //    else
   //       cout << content;
   //    }
   // return *this;
   //}

   }
}