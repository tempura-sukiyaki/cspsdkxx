/// \file triglavpluginsdk.hh
///
/// \copyright (C) 2017 tempura-sukiyaki
///

#ifndef cspsdkxx_triglavpluginsdk_trigravpluginsdk_hh_
#define cspsdkxx_triglavpluginsdk_trigravpluginsdk_hh_

#if !defined( TP_ASSERT )
# include <cassert>
# define TP_ASSERT( condition ) assert( condition )
#endif // !defined( TP_ASSERT )

#define TP_EXTERN_C_START TRIGLAV_PLUGIN_EXTERN_C_START
#define TP_EXTERN_C_END TRIGLAV_PLUGIN_EXTERN_C_END
#define TP_API TRIGLAV_PLUGIN_API
#define TP_CALLBACK TRIGLAV_PLUGIN_CALLBACK

#if defined( TRIGLAV_PLUGIN_ACTIVATION )
# define TP_ACTIVATION 1
#else
# define TP_ACTIVATION 0
#endif // !defined( TRIGLAV_PLUGIN_ACTIVATION )

#include <memory>
#include <string>
#include <tuple>

#include <TriglavPlugInSDK/TriglavPlugInSDK.h>


namespace Triglav { namespace PlugIn {


using Bool = TriglavPlugInBool;
using Boolean = Bool;

constexpr static Bool kBoolTrue = kTriglavPlugInBoolTrue; 
constexpr static Bool kBoolFalse = kTriglavPlugInBoolFalse;


using Char = TriglavPlugInChar;


using UniChar = TriglavPlugInUniChar;


using UInt8 = TriglavPlugInUInt8;

enum class Byte : UInt8 {};

constexpr auto toByte( UInt8 x ) noexcept -> Byte
  { return static_cast< Byte >( x ); }


using Int = TriglavPlugInInt;
using Integer = Int;

enum class StringId : Int {};

constexpr auto toStringId( Int x ) noexcept -> StringId
  { return static_cast< StringId >( x ); }


using Int64 = TriglavPlugInInt64;


using Float = TriglavPlugInFloat;


using Double = TriglavPlugInDouble;
using Decimal = Double;


using Ptr = TriglavPlugInPtr;


using Point = TriglavPlugInPoint;


using Size = TriglavPlugInSize;


using Rect = TriglavPlugInRect;


using RGBColor = TriglavPlugInRGBColor;


using CMYKColor = TriglavPlugInCMYKColor;


using Server = TriglavPlugInServer;


using HostObject = TriglavPlugInHostObject;
using StringObject = TriglavPlugInStringObject;
using BitmapObject = TriglavPlugInBitmapObject;
using OffscreenObject = TriglavPlugInOffscreenObject;
using PropertyObject = TriglavPlugInPropertyObject;


using RecordSuite = TriglavPlugInRecordSuite;
using ModuleInitializeRecord = TriglavPlugInModuleInitializeRecord;
#if TP_ACTIVATION
using FilterInitializeRecord = TriglavPlugInFilterActivationInitializeRecord;
using FilterRunRecord = TriglavPlugInFilterActivationRunRecord;
#else
using FilterInitializeRecord = TriglavPlugInFilterInitializeRecord;
using FilterRunRecord = TriglavPlugInFilterRunRecord;
#endif // TP_ACTIVATION


using ServiceSuite = TriglavPlugInServiceSuite;
using StringService = TriglavPlugInStringService;
using BitmapService = TriglavPlugInBitmapService;
using OffscreenService = TriglavPlugInOffscreenService;
using OffscreenService2 = TriglavPlugInOffscreenService2;
using PropertyService = TriglavPlugInPropertyService;
using PropertyService2 = TriglavPlugInPropertyService2;


enum class Selectors : Int
  {
    ModuleInitialize = kTriglavPlugInSelectorModuleInitialize,
    ModuleTerminate = kTriglavPlugInSelectorModuleTerminate,
    FilterInitialize = kTriglavPlugInSelectorFilterInitialize,
    FilterTerminate = kTriglavPlugInSelectorFilterTerminate,
    FilterRun = kTriglavPlugInSelectorFilterRun,
  };


enum class CallResults : Int
  {
    Success = kTriglavPlugInCallResultSuccess,
    Failed = kTriglavPlugInCallResultFailed,
  };

constexpr auto isSuccess( CallResults result ) noexcept -> bool
  { return result == CallResults::Success; }


enum class APIResults : Int
  {
    Success = kTriglavPlugInAPIResultSuccess,
    Failed = kTriglavPlugInAPIResultFailed,
  };

constexpr auto toAPIResults( Int result ) noexcept -> APIResults
  { return static_cast< APIResults >( result ); }

constexpr auto isSuccess( APIResults result ) noexcept -> bool
  { return result == APIResults::Success; }


template < class T >
class APIResult
  {
  public:
    template < class Y > friend class APIResult;

    using Value = T;

    ~APIResult() { unset(); }
    constexpr APIResult() noexcept
      : state_{ APIResults::Failed }, dummy_{} {}
    constexpr APIResult( APIResult const &x ) { set( x ); }
    constexpr APIResult( APIResult &&x ) { set( std::move( x ) ); }

    constexpr auto operator =( APIResult const &x ) -> APIResult &
      {
        unset();
        set( x );
        return *this;
      }

    constexpr auto operator =( APIResult &&x ) -> APIResult &
      {
        unset();
        set( std::move( x ) );
        return *this;
      }

    constexpr APIResult( APIResults x )
      : state_{ x }, dummy_{} {}

    constexpr APIResult( Value const &x )
      : state_{ APIResults::Success }, dummy_{}
      { new ( &value_ ) Value{ x }; }

    constexpr APIResult( Value &&x )
      : state_{ APIResults::Success }, dummy_{}
      { new ( &value_ ) Value{ std::move( x ) }; }

    constexpr APIResult( APIResults s, Value const &x )
      : state_{ s }, dummy_{}
      {
        if ( isSuccess( state_ ) )
          { new ( &value_ ) Value{ x }; }
      }

    constexpr APIResult( APIResults s, Value &&x )
      : state_{ s }, dummy_{}
      {
        if ( isSuccess( state_ ) )
          { new ( &value_ ) Value{ std::move( x ) }; }
      }

    template < class Y >
    constexpr APIResult( APIResult< Y > &&x )
      : state_{ x.state_ }, dummy_{}
      {
        if ( isSuccess( state_ ) )
          { new ( &value_ ) Value{ std::forward< Y >( x.value_ ) }; }
      }

    constexpr explicit operator bool() const noexcept
      { return isSuccess( state_ ); }

    constexpr auto operator *() const & noexcept -> Value const &
      { return value_; }

    constexpr auto operator *() && noexcept -> Value
      { return std::move( value_ ); }

    constexpr auto operator->() const & noexcept -> Value const *
      { return &value_; }

    constexpr auto operator->() && noexcept -> Value const * = delete;

    constexpr auto state() const noexcept -> APIResults
      { return state_; }

    constexpr auto value() const & noexcept -> Value const &
      { return value_; }

    constexpr auto value() && noexcept -> Value
      { return std::move( value_ ); }

    constexpr void reset( APIResults s )
      {
        unset();
        state_ = s;
      }

    constexpr void reset( APIResults s, Value const &x )
      {
        unset();
        state_ = s;
        if ( isSuccess( state_ ) )
          { new ( &value_ ) Value{ x }; }
      }

    constexpr void reset( APIResults s, Value &&x )
      {
        unset();
        state_ = s;
        if ( isSuccess( state_ ) )
          { new ( &value_ ) Value{ std::move( x ) }; }
      }

  private:
    constexpr void set( APIResult const &x )
      {
        state_ = x.state_;
        if ( isSuccess( state_ ) )
          { new ( &value_ ) Value{ x.value_ }; }
      }
    constexpr void set( APIResult &&x )
      {
        state_ = x.state_;
        if ( isSuccess( state_ ) )
          { new ( &value_ ) Value{ std::move( x.value_ ) }; }
      }
    constexpr void unset() noexcept
      {
        if ( isSuccess( state_ ) )
          { value_.~Value(); }
        dummy_ = false;
      }

    APIResults state_;
    union { bool dummy_; T value_; };
  };

template <>
class APIResult< void >
  {
  public:
    ~APIResult() = default;
    constexpr APIResult() noexcept : state_{ APIResults::Failed } {}
    APIResult( APIResult const & ) = default;
    APIResult( APIResult && ) = default;
    auto operator =( APIResult const & ) -> APIResult & = default;
    auto operator =( APIResult && ) -> APIResult & = default;

    constexpr APIResult( APIResults x ) noexcept
      : state_{ x } {}

    constexpr explicit operator bool() const noexcept
      { return isSuccess( state_ ); }

    constexpr auto state() const noexcept -> APIResults
      { return state_; }

    constexpr void reset( APIResults x )
      { state_ = x; }

  private:
    APIResults state_;
  };


class RecordBase
  {
  protected:
    ~RecordBase() = default;
  public:
    RecordBase() = default;
    RecordBase( RecordBase const & ) = default;
    RecordBase( RecordBase && ) = default;
    auto operator =( RecordBase const & ) -> RecordBase & = default;
    auto operator =( RecordBase && ) -> RecordBase & = default;

    explicit RecordBase( std::weak_ptr< Server const > const &server )
      : server_{ server } {}

    explicit operator bool() const noexcept
      { return !server_.expired(); }

    auto server() const noexcept -> Server const *
      { return server_.lock().get(); }

    auto suite() const noexcept -> RecordSuite const *
      {
        if ( auto const pserver = server() )
          { return &pserver->recordSuite; }
        return nullptr;
      }

    auto host() const noexcept -> HostObject
      {
        if ( auto pserver = server() )
          { return pserver->hostObject; }
        return nullptr;
      }

  private:
    std::weak_ptr< Server const > server_;
  };


template < class Object_ >
class ServiceBase
  {
  public:
    using Object = Object_;

  protected:
    ~ServiceBase() = default;
  public:
    ServiceBase() = default;
    ServiceBase( ServiceBase const & ) = default;
    ServiceBase( ServiceBase && ) = default;
    auto operator =( ServiceBase const & ) -> ServiceBase & = default;
    auto operator =( ServiceBase && ) -> ServiceBase & = default;

    constexpr explicit ServiceBase( std::weak_ptr< Server const > const &server ) noexcept
      : server_{ server }, object_{} {}

    constexpr explicit operator bool() const noexcept
      { return !server_.expired() && object_; }

    constexpr operator Object() const noexcept
      { return object_.get(); }

    constexpr auto object() const noexcept -> Object
      { return object_.get(); }

    constexpr auto server() const noexcept -> Server const *
      { return server_.lock().get(); }

    constexpr auto suite() const noexcept -> ServiceSuite const *
      {
        if ( auto const pserver = server() )
          { return &pserver->serviceSuite; }
        return nullptr;
      }

    constexpr auto host() const noexcept -> HostObject
      {
        if ( auto const pserver = server() )
          { return pserver->hostObject; }
        return nullptr;
      }

    template < class Service >
    constexpr void reset( Service const &service, Object x, bool owned )
      {
        if ( !owned )
          { service.retainProc( x ); }
        object_.reset( x, service.releaseProc );
      }

  private:
    std::weak_ptr< Server const > server_;
    std::shared_ptr< std::remove_pointer_t< Object_ > > object_;
  };


class String : public ServiceBase< StringObject >
  {
  public:
    class Data
      {
      public:
        enum class Types : std::uint8_t
          {
            StringId,
            AsciiString,
            LocaleCodeString,
            UnicodeString,
          };

        ~Data() { unset(); }

        Data() : type_{ Types::AsciiString } { new ( &string8_ ) std::string{}; }
        Data( Data const &x ) { set( x ); }
        Data( Data &&x ) noexcept { set( std::move( x ) ); }

        auto operator =( Data const &x ) -> Data &
          {
            unset();
            set( x );
            return *this;
          }

        auto operator =( Data &&x ) noexcept -> Data &
          {
            unset();
            set( std::move( x ) );
            return *this;
          }

        constexpr Data( StringId x ) noexcept
          : type_{ Types::StringId }, stringId_{ x } {}

        Data( std::string const &x, bool locale )
          : type_{ locale ? Types::LocaleCodeString : Types::AsciiString }
          , stringId_{}
          { new ( &string8_ ) std::string{ x }; }

        Data( std::string &&x, bool locale ) noexcept
          : type_{ locale ? Types::LocaleCodeString : Types::AsciiString }
          , stringId_{}
          { new ( &string8_ ) std::string{ std::move( x ) }; }

        Data( std::u16string const &x )
          : type_{ Types::UnicodeString }
          , stringId_{}
          { new ( &string16_ ) std::u16string{ x }; }

        Data( std::u16string &&x ) noexcept
          : type_{ Types::UnicodeString }
          , stringId_{}
          { new ( &string16_ ) std::u16string{ std::move( x ) }; }

        auto type() const noexcept -> Types { return type_; }

        auto stringId() const noexcept -> StringId
          {
            TP_ASSERT( type_ == Types::StringId );
            return stringId_;
          }

        auto asciiString() const noexcept -> std::string const &
          {
            TP_ASSERT( type_ == Types::AsciiString );
            return string8_;
          }

        auto localCodeString() const noexcept -> std::string const &
          {
            TP_ASSERT( type_ == Types::LocaleCodeString );
            return string8_;
          }

        auto unicodeString() const noexcept -> std::u16string const &
          {
            TP_ASSERT( type_ == Types::UnicodeString );
            return string16_;
          }

      private:
        void set( Data const &x )
          {
            type_ = x.type_;
            switch ( type_ )
              {
                case Types::StringId:
                    stringId_ = x.stringId_;
                    break;
                case Types::AsciiString:
                case Types::LocaleCodeString:
                    new ( &string8_ ) std::string{ x.string8_ };
                    break;
                case Types::UnicodeString:
                    new ( &string16_ ) std::u16string{ x.string16_ };
                    break;
              }
          }

        void set( Data &&x ) noexcept
          {
            type_ = x.type_;
            switch ( type_ )
              {
                case Types::StringId:
                    stringId_ = std::move( x.stringId_ );
                    break;
                case Types::AsciiString:
                case Types::LocaleCodeString:
                    new ( &string8_ ) std::string{ std::move( x.string8_ ) };
                    break;
                case Types::UnicodeString:
                    new ( &string16_ ) std::u16string{ std::move( x.string16_ ) };
                    break;
              }
          }

        void unset() noexcept
          {
            switch ( type_ )
              {
                case Types::StringId:
                    break;
                case Types::AsciiString:
                case Types::LocaleCodeString:
                    string8_.~basic_string();
                    break;
                case Types::UnicodeString:
                    string16_.~basic_string();
                    break;
              }
          }

        Types type_;
        union
          {
            StringId stringId_;
            std::string string8_;
            std::u16string string16_;
          };
      };

    using Service = StringService;

    ~String() = default;
    String() = default;
    String( String const & ) = default;
    String( String && ) = default;
    auto operator =( String const & ) -> String & = default;
    auto operator =( String && ) -> String & = default;

    explicit String( std::weak_ptr< Server const > const &server ) noexcept
      : ServiceBase{ server } {}

    auto service() const noexcept -> Service const *
      {
        if ( auto const psuite = suite() )
          { return psuite->stringService; }
        return nullptr;
      }

    auto makeWithAsciiString( std::string const &str ) -> APIResult< void >
      {
        APIResult< void > result{};
        if ( auto const pservice = service() )
          {
            Object x{};
            auto const a0 = str.c_str();
            auto const a1 = static_cast< Int >( str.length() );
            auto const q = toAPIResults( pservice->createWithAsciiStringProc( &x, a0, a1 ) );
            result.reset( q );
            if ( result )
              { reset( *pservice, x, true ); }
          }
        return result;
      }

    auto makeWithUnicodeString( std::u16string const &str ) -> APIResult< void >
      {
        APIResult< void > result{};
        if ( auto const pservice = service() )
          {
            Object x{};
            auto const a0 = reinterpret_cast< UniChar const * >( str.c_str() );
            auto const a1 = static_cast< Int >( str.length() );
            auto const q = toAPIResults( pservice->createWithUnicodeStringProc( &x, a0, a1 ) );
            result.reset( q );
            if ( result )
              { reset( *pservice, x, true ); }
          }
        return result;
      }

    auto makeWithLocalCodeString( std::string const &str ) -> APIResult< void >
      {
        APIResult< void > result{};
        if ( auto const pservice = service() )
          {
            Object x{};
            auto const a0 = str.c_str();
            auto const a1 = static_cast< Int >( str.length() );
            auto const q = toAPIResults( pservice->createWithLocalCodeStringProc( &x, a0, a1 ) );
            result.reset( q ); 
            if ( result )
              { reset( *pservice, x, true ); }
          }
        return result;
      }

    auto makeWithStringId( StringId stringId ) -> APIResult< void >
      {
        APIResult< void > result{};
        if ( auto const pservice = service() )
          {
            Object x{};
            auto const a0 = static_cast< Int >( stringId );
            auto const a1 = host();
            auto const q = toAPIResults( pservice->createWithStringIDProc( &x, a0, a1 ) );
            result.reset( q );
            if ( result )
              { reset( *pservice, x, true ); }
          }
        return result;
      }

    auto getUnicodeString() const -> APIResult< std::u16string >
      {
        APIResult< std::u16string > result{};
        if ( auto const pservice = service() )
          {
            UniChar const *x0{};
            auto q = toAPIResults( pservice->getUnicodeCharsProc( &x0, *this ) );
            if ( isSuccess( q ) )
              {
                Int x1{};
                q = toAPIResults( pservice->getUnicodeLengthProc( &x1, *this ) );
                if ( isSuccess( q ) )
                  { result.reset( q, std::u16string{ reinterpret_cast< char16_t const * >( x0 ), static_cast< size_t >( x1 ) }); }
                else
                  { result.reset( q ); }
              }
          }
        return result;
      }

    auto getLocalCodeString() const -> APIResult< std::string >
      {
        APIResult< std::string > result{};
        if( auto const pservice = service() )
          {
            Char const *x0{};
            auto q = toAPIResults( pservice->getLocalCodeCharsProc( &x0, *this ) );
            if ( isSuccess( q ) )
              {
                Int x1{};
                q = toAPIResults( pservice->getLocalCodeLengthProc( &x1, *this ) );
                if ( isSuccess( q ) )
                  { result.reset( q, std::string{ x0, static_cast< size_t >( x1 ) } ); }
                else
                  { result.reset( q ); }
              }
          }
        return result;
      }
  };

inline auto makeStringWithObject( std::weak_ptr< Server const > const &server, StringObject object, bool owned ) noexcept -> String
  {
    String result{ server };
    if ( auto const pservice = result.service() )
      { result.reset( *pservice, object, owned ); }
    return result;
  }

inline auto makeStringWithAsciiString( std::weak_ptr< Server const > const &server, std::string const &x ) -> String
  {
    String result{ server };
    result.makeWithAsciiString( x );
    return result;
  }

inline auto makeStringWithUnicodeString( std::weak_ptr< Server const > const &server, std::u16string const &x ) -> String
  {
    String result{ server };
    result.makeWithUnicodeString( x );
    return result;
  }

inline auto makeStringWithLocalCodeString( std::weak_ptr< Server const > const &server, std::string const &x ) -> String
  {
    String result{ server };
    result.makeWithLocalCodeString( x );
    return result;
  }

inline auto makeStringWithStringId( std::weak_ptr< Server const > const &server, StringId x ) -> String
  {
    String result{ server };
    result.makeWithStringId( x );
    return result;
  }

inline auto makeStringWithData( std::weak_ptr< Server const > const &server, String::Data const &x ) -> String
  {
    String result{ server };
    switch ( x.type() )
      {
        case String::Data::Types::StringId:
            result.makeWithStringId( x.stringId() );
            break;
        case String::Data::Types::AsciiString:
            result.makeWithAsciiString( x.asciiString() );
            break;
        case String::Data::Types::LocaleCodeString:
            result.makeWithLocalCodeString( x.localCodeString() );
            break;
        case String::Data::Types::UnicodeString:
            result.makeWithUnicodeString( x.unicodeString() );
            break;
      }
    return result;
  }


class Bitmap : public ServiceBase< BitmapObject >
  {
  public:
    using Service = TriglavPlugInBitmapService;

    enum class Scanlines : Int
      {
        HorizontalLeftTop = kTriglavPlugInBitmapScanlineHorizontalLeftTop,
        HorizontalRightTop = kTriglavPlugInBitmapScanlineHorizontalRightTop,
        HorizontalLeftBottom = kTriglavPlugInBitmapScanlineHorizontalLeftBottom,
        HorizontalRightBottom = kTriglavPlugInBitmapScanlineHorizontalRightBottom,
        VerticalLeftTop = kTriglavPlugInBitmapScanlineVerticalLeftTop,
        VerticalRightTop = kTriglavPlugInBitmapScanlineVerticalRightTop,
        VerticalLeftBottom = kTriglavPlugInBitmapScanlineVerticalLeftBottom,
        VerticalRightBottom = kTriglavPlugInBitmapScanlineVerticalRightBottom,
      };

    ~Bitmap() = default;
    Bitmap() = default;
    Bitmap( Bitmap const & ) = default;
    Bitmap( Bitmap && ) = default;
    auto operator=( Bitmap const & ) -> Bitmap & = default;
    auto operator=( Bitmap && ) -> Bitmap & = default;

    explicit Bitmap( std::weak_ptr< Server const > const &server ) noexcept
      : ServiceBase{ server } {}

    auto service() const noexcept -> Service const *
      {
        if ( auto const psuite = suite() )
          { return psuite->bitmapService; }
        return nullptr;
      }

    auto make( Int width, Int height, Int depth, Scanlines scanline ) -> APIResult< void >
      {
        APIResult< void > result{};
        if ( auto const pservice = service() )
          {
            Object x{};
            auto const a = static_cast< Int >( scanline );
            auto const q = toAPIResults( pservice->createProc( &x, width, height, depth, a ) );
            result.reset( q );
            if ( result )
              { reset( *pservice, x, true ); }
          }
        return result;
      }

    auto getWidth() const noexcept -> APIResult< Int >
      {
        APIResult< Int > result{};
        if ( auto const pservice = service() )
          {
            Int x{};
            auto const q = toAPIResults( pservice->getWidthProc( &x, *this ) );
            result.reset( q, x );
          }
        return result;
      }

    auto getHeight() const noexcept -> APIResult< Int >
      {
        APIResult< Int > result{};
        if ( auto const pservice = service() )
          {
            Int x{};
            auto const q = toAPIResults( pservice->getHeightProc( &x, *this ) );
            result.reset( q, x );
          }
        return result;
      }

    auto getDepth() const noexcept -> APIResult< Int >
      {
        APIResult< Int > result{};
        if ( auto const pservice = service() )
          {
            Int x{};
            auto const q = toAPIResults( pservice->getDepthProc( &x, *this ) );
            result.reset( q, x );
          }
        return result;
      }

    auto getScanline() const noexcept -> APIResult< Scanlines >
      {
        APIResult< Scanlines > result{};
        if ( auto const pservice = service() )
          {
            Int x{};
            auto const q = toAPIResults( pservice->getScanlineProc( &x, *this ) );
            result.reset( q, static_cast< Scanlines >( x ) );
          }
        return result;
      }

    auto getMutableAddress( Point const &pos ) const noexcept -> APIResult< Byte * >
      {
        APIResult< Byte * > result{};
        if ( auto const pservice = service() )
          {
            Ptr x{};
            auto const q = toAPIResults( pservice->getAddressProc( &x, *this, &pos ) );
            result.reset( q, static_cast< Byte * >( x ) );
          }
        return result;
      }

    auto getAddress( Point const &pos ) const noexcept -> APIResult< Byte const * >
      { return getMutableAddress( pos ); }

    auto getRowBytes() const noexcept -> APIResult< Int >
      {
        APIResult< Int > result{};
        if ( auto const pservice = service() )
          {
            Int x{};
            auto const q = toAPIResults( pservice->getRowBytesProc( &x, *this ) );
            result.reset( q, x );
          }
        return result;
      }

    auto getPixelBytes() const noexcept -> APIResult< Int >
      {
        APIResult< Int > result{};
        if ( auto const pservice = service() )
          {
            Int x{};
            auto const q = toAPIResults( pservice->getPixelBytesProc( &x, *this ) );
            result.reset( q, x );
          }
        return result;
      }
  };

inline auto makeBitmapWithObject( std::weak_ptr< Server const > const &server, BitmapObject object, bool owned ) -> Bitmap
  {
    Bitmap result{ server };
    if ( auto const pservice = result.service() )
      { result.reset( *pservice, object, owned ); }
    return result;
  }

inline auto makeBitmap( std::weak_ptr< Server const > const &server, Int width, Int height, Int depth, Bitmap::Scanlines scanline ) -> Bitmap
  {
    Bitmap result{ server };
    result.make( width, height, depth, scanline );
    return result;
  }


class Offscreen : public ServiceBase< OffscreenObject >
  {
  public:
    enum class ChannelOrders : Int
      {
        Alpha = kTriglavPlugInOffscreenChannelOrderAlpha,
        GrayAlpha = kTriglavPlugInOffscreenChannelOrderGrayAlpha,
        RGBAlpha = kTriglavPlugInOffscreenChannelOrderRGBAlpha,
        CMYKAlpha = kTriglavPlugInOffscreenChannelOrderCMYKAlpha,
        BinarizationAlpha = kTriglavPlugInOffscreenChannelOrderBinarizationAlpha,
        BinarizationGrayAlpha = kTriglavPlugInOffscreenChannelOrderBinarizationGrayAlpha,
        SelectArea = kTriglavPlugInOffscreenChannelOrderSelectArea,
        Plane = kTriglavPlugInOffscreenChannelOrderPlane,
      };

    enum class CopyModes : Int
      {
        Normal = kTriglavPlugInOffscreenCopyModeNormal,
        Image = kTriglavPlugInOffscreenCopyModeImage,
        Gray = kTriglavPlugInOffscreenCopyModeGray,
        Red = kTriglavPlugInOffscreenCopyModeRed,
        Green = kTriglavPlugInOffscreenCopyModeGreen,
        Blue = kTriglavPlugInOffscreenCopyModeBlue,
        Cyan = kTriglavPlugInOffscreenCopyModeCyan,
        Magenta = kTriglavPlugInOffscreenCopyModeMagenta,
        Yellow = kTriglavPlugInOffscreenCopyModeYellow,
        KeyPlate = kTriglavPlugInOffscreenCopyModeKeyPlate,
        Alpha = kTriglavPlugInOffscreenCopyModeAlpha,
      };

    struct MutableBlock
      {
        Byte *address;
        Int rowBytes;
        Int pixelBytes;
        Rect rect;

        ~MutableBlock() = default;
        MutableBlock() = default;
        MutableBlock( MutableBlock const & ) = default;
        MutableBlock( MutableBlock && ) = default;
        auto operator =( MutableBlock const & ) -> MutableBlock & = default;
        auto operator =( MutableBlock && ) -> MutableBlock & = default;
      };

    struct Block
      {
        Byte const *address;
        Int rowBytes;
        Int pixelBytes;
        Rect rect;

        ~Block() = default;
        Block() = default;
        Block( Block const & ) = default;
        Block( Block && ) = default;
        auto operator =( Block const & ) -> Block & = default;
        auto operator =( Block && ) -> Block & = default;

        constexpr Block( MutableBlock const &x )
          : address{ x.address }
          , rowBytes{ x.rowBytes }
          , pixelBytes{ x.pixelBytes }
          , rect{ x.rect }
          {}

        constexpr Block( MutableBlock &&x )
          : address{ std::move( x.address ) }
          , rowBytes{ std::move( x.rowBytes ) }
          , pixelBytes{ std::move( x.pixelBytes ) }
          , rect{ std::move( x.rect ) }
          {}

        constexpr auto operator =( MutableBlock const &x ) -> Block &
          {
            *this = Block{ x };
            return *this;
          }
        
        constexpr auto operator =( MutableBlock &&x ) -> Block &
          {
            *this = Block{ std::move( x ) };
            return *this;
          }
      };

    using Service = OffscreenService;
    using Service2 = OffscreenService2;

    ~Offscreen() = default;
    Offscreen() = default;
    Offscreen( Offscreen const & ) = default;
    Offscreen( Offscreen && ) = default;
    auto operator =( Offscreen const & ) -> Offscreen & = default;
    auto operator =( Offscreen && ) -> Offscreen & = default;

    explicit Offscreen( std::weak_ptr< Server const > const &server ) noexcept
      : ServiceBase{ server } {}

    auto service() const noexcept -> Service const *
      {
        if ( auto const psuite = suite() )
          { return psuite->offscreenService; }
        return nullptr;
      }
    auto service2() const noexcept -> Service2 const *
      {
        if ( auto const psuite = suite() )
          { return psuite->offscreenService2; }
        return nullptr;
      }

    auto makePlane( Int width, Int height, Int depth ) -> APIResult< void >
      {
        APIResult< void > result{};
        if ( auto const pservice = service() )
          {
            Object x{};
            auto const q = toAPIResults( pservice->createPlaneProc( &x, width, height, depth ) );
            result.reset( q );
            if( result )
              { reset( *pservice, x, true ); }
          }
        return result;
      }

    auto getWidth() const noexcept -> APIResult< Int >
      {
        APIResult< Int > result{};
        if ( auto const pservice = service() )
          {
            Int x{};
            auto const q = toAPIResults( pservice->getWidthProc( &x, *this ) );
            result.reset( q, x );
          }
        return result;
      }

    auto getHeight() const noexcept -> APIResult< Int >
      {
        APIResult< Int > result{};
        if ( auto const pservice = service() )
          {
            Int x{};
            auto const q = toAPIResults( pservice->getHeightProc( &x, *this ) );
            result.reset( q, x );
          }
        return result;
      }

    auto getRect() const noexcept -> APIResult< Rect >
      {
        APIResult< Rect > result{};
        if ( auto const pservice = service() )
          {
            Rect x{};
            auto const q = toAPIResults( pservice->getRectProc( &x, *this ) );
            result.reset( q, std::move( x ) );
          }
        return result;
      }

    auto getExtentRect() const noexcept -> APIResult< Rect >
      {
        APIResult< Rect > result{};
        if ( auto const pservice = service() )
          {
            Rect x{};
            auto const q = toAPIResults( pservice->getExtentRectProc( &x, *this ) );
            result.reset( q, std::move( x ) );
          }
        return result;
      }

    auto getChannelOrder() const noexcept -> APIResult< ChannelOrders >
      {
        APIResult< ChannelOrders > result{};
        if ( auto const pservice = service() )
          {
            Int x{};
            auto const q = toAPIResults( pservice->getChannelOrderProc( &x, *this ) );
            result.reset( q, static_cast< ChannelOrders >( x ) );
          }
        return result;
      }

    auto getRGBChannelIndex() const noexcept -> APIResult< std::tuple< Int, Int, Int > >
      {
        APIResult< std::tuple< Int, Int, Int > > result{};
        if ( auto const pservice = service() )
          {
            Int r{}, g{}, b{};
            auto const q = toAPIResults( pservice->getRGBChannelIndexProc( &r, &g, &b, *this ) );
            result.reset( q, std::make_tuple( r, g, b ) );
          }
        return result;
      }

    auto getCMYKChannelIndex() const noexcept -> APIResult< std::tuple< Int, Int, Int, Int > >
      {
        APIResult< std::tuple< Int, Int, Int, Int > > result{};
        if ( auto const pservice = service() )
          {
            Int c{}, m{}, y{}, k{};
            auto const q = toAPIResults( pservice->getCMYKChannelIndexProc( &c, &m, &y, &k, *this ) );
            result.reset( q, std::make_tuple( c, m, y, k ) );
          }
        return result;
      }

    auto getBlockRectCount( Rect const &bounds ) const noexcept -> APIResult< Int >
      {
        APIResult< Int > result{};
        if ( auto const pservice = service() )
          {
            Int x{};
            auto a = bounds;
            auto const q = toAPIResults( pservice->getBlockRectCountProc( &x, *this, &a ) );
            result.reset( q, x );
          }
        return result;
      }

    auto getBlockRect( Int index, Rect const &bounds ) const noexcept -> APIResult< Rect >
      {
        APIResult< Rect > result{};
        if ( auto const pservice = service() )
          {
            Rect x{};
            auto a = bounds;
            auto const q = toAPIResults( pservice->getBlockRectProc( &x, index, *this, &a ) );
            result.reset( q, std::move( x ) );
          }
        return result;
      }

    auto getMutableBlockImage( Point const &pos ) const noexcept -> APIResult< MutableBlock >
      {
        APIResult< MutableBlock > result{};
        if ( auto const pservice = service() )
          {
            MutableBlock x{};
            auto a0 = reinterpret_cast< Ptr * >( &x.address );
            auto a1 = pos;
            auto const q = toAPIResults( pservice->getBlockImageProc( a0, &x.rowBytes, &x.pixelBytes, &x.rect, *this, &a1 ) );
            result.reset( q, std::move( x ) );
          }
        return result;
      }

    auto getMutableBlockAlpha( Point const &pos ) const noexcept -> APIResult< MutableBlock >
      {
        APIResult< MutableBlock > result{};
        if ( auto const pservice = service() )
          {
            MutableBlock x{};
            auto a0 = reinterpret_cast< Ptr * >( &x.address );
            auto a1 = pos;
            auto const q = toAPIResults( pservice->getBlockAlphaProc( a0, &x.rowBytes, &x.pixelBytes, &x.rect, *this, &a1 ) );
            result.reset( q, std::move( x ) );
          }
        return result;
      }

    auto getMutableBlockSelectArea( Point const &pos ) const noexcept -> APIResult< MutableBlock >
      {
        APIResult< MutableBlock > result{};
        if ( auto const pservice = service() )
          {
            MutableBlock x{};
            auto a0 = reinterpret_cast< Ptr * >( &x.address );
            auto a1 = pos;
            auto const q = toAPIResults( pservice->getBlockSelectAreaProc( a0, &x.rowBytes, &x.pixelBytes, &x.rect, *this, &a1 ) );
            result.reset( q, std::move( x ) );
          }
        return result;
      }

    auto getMutableBlockPlane( Point const &pos ) const noexcept -> APIResult< MutableBlock >
      {
        APIResult< MutableBlock > result{};
        if ( auto const pservice = service() )
          {
            MutableBlock x{};
            auto a0 = reinterpret_cast< Ptr * >( &x.address );
            auto a1 = pos;
            auto const q = toAPIResults( pservice->getBlockPlaneProc( a0, &x.rowBytes, &x.pixelBytes, &x.rect, *this, &a1 ) );
            result.reset( q, std::move( x ) );
          }
        return result;
      }

    auto getBlockImage( Point const &pos ) const noexcept -> APIResult< Block >
      { return getMutableBlockImage( pos ); }

    auto getBlockAlpha( Point const &pos ) const noexcept -> APIResult< Block >
      { return getMutableBlockAlpha( pos ); }

    auto getBlockSelectArea( Point const &pos ) const noexcept -> APIResult< Block >
      { return getMutableBlockSelectArea( pos ); }

    auto getBlockPlane( Point const &pos ) const noexcept -> APIResult< Block >
      { return getMutableBlockPlane( pos ); }

    auto getTileWidth() const noexcept -> APIResult< Int >
      {
        APIResult< Int > result{};
        if ( auto const pservice = service() )
          {
            Int x{};
            auto const q = toAPIResults( pservice->getTileWidthProc( &x, *this ) );
            result.reset( q, x );
          }
        return result;
      }

    auto getTileHeight() const noexcept -> APIResult< Int >
      {
        APIResult< Int > result{};
        if ( auto const pservice = service() )
          {
            Int x{};
            auto const q = toAPIResults( pservice->getTileHeightProc( &x, *this ) );
            result.reset( q, x );
          }
        return result;
      }

    auto getBitmap( BitmapObject bitmap, Point const &bitmapPos, Point const &offscreenPos, Int width, Int height, CopyModes mode ) const noexcept -> APIResult< void >
      {
        APIResult< void > result{};
        if ( auto const pservice = service() )
          {
            auto const a = static_cast< Int >( mode );
            auto const q = toAPIResults( pservice->getBitmapProc( bitmap, &bitmapPos, *this, &offscreenPos, width, height, a ) );
            result.reset( q );
          }
        return result;
    }
    auto setBitmap( Point const &offscreenPos, BitmapObject bitmap, Point const &bitmapPos, Int width, Int height, CopyModes mode ) const noexcept -> APIResult< void >
      {
        APIResult< void > result{};
        if ( auto const pservice = service() )
          {
            auto const a = static_cast< Int >( mode );
            auto const q = toAPIResults( pservice->setBitmapProc( *this, &offscreenPos, bitmap, &bitmapPos, width, height, a ) );
            result.reset( q );
          }
        return result;
      }

    auto getBitmapNormalAlphaChannelIndex( BitmapObject bitmap ) const noexcept -> APIResult< Int >
      {
        APIResult< Int > result{};
        if ( auto const pservice = service2() )
          {
            Int x{};
            auto const q = toAPIResults( pservice->getBitmapNormalAlphaChannelIndexProc( &x, *this ) );
            result.reset( q, x );
          }
        return result;
      }
  };

inline auto makeOffscreenWithObject( std::weak_ptr< Server const > const &server, OffscreenObject object, bool owned ) -> Offscreen
  {
    Offscreen result{ server };
    if ( auto const pservice = result.service() )
      { result.reset( *pservice, object, owned ); }
    return result;
  }

inline auto makePlaneOffscreen( std::weak_ptr< Server const > const &server, Int width, Int height, Int depth ) -> Offscreen
  {
    Offscreen result{ server };
    result.makePlane( width, height, depth );
    return result;
  }


class Property : public ServiceBase< PropertyObject >
  {
  public:
    using Service = PropertyService;
    using Service2 = PropertyService2;

    using CallBackProc = TriglavPlugInPropertyCallBackProc;

    enum class ValueTypes : Int
      {
        Void = kTriglavPlugInPropertyValueTypeVoid,
        Boolean = kTriglavPlugInPropertyValueTypeBoolean,
        Enumeration = kTriglavPlugInPropertyValueTypeEnumeration,
        Integer = kTriglavPlugInPropertyValueTypeInteger,
        Decimal = kTriglavPlugInPropertyValueTypeDecimal,
        Point = kTriglavPlugInPropertyValueTypePoint,
        String = kTriglavPlugInPropertyValueTypeString,
      };

    enum class InputKinds : Int
      {
        Hide = kTriglavPlugInPropertyInputKindHide,
        Default = kTriglavPlugInPropertyInputKindDefault,
        PushButton = kTriglavPlugInPropertyInputKindPushButton,
        Canvas = kTriglavPlugInPropertyInputKindCanvas,
      };

    enum class ValueKinds : Int
      {
        Default = kTriglavPlugInPropertyValueKindDefault,
        Pixel = kTriglavPlugInPropertyValueKindPixel,
      };

    enum class PointDefaultValueKinds : Int
      {
        Default = kTriglavPlugInPropertyPointDefaultValueKindDefault,
        CanvasLeftTop = kTriglavPlugInPropertyPointDefaultValueKindCanvasLeftTop,
        CanvasRightTop = kTriglavPlugInPropertyPointDefaultValueKindCanvasRightTop,
        CanvasLeftBottom = kTriglavPlugInPropertyPointDefaultValueKindCanvasLeftBottom,
        CanvasRightBottom = kTriglavPlugInPropertyPointDefaultValueKindCanvasRightBottom,
        CanvasCenter = kTriglavPlugInPropertyPointDefaultValueKindCanvasCenter,
        SelectAreaLeftTop = kTriglavPlugInPropertyPointDefaultValueKindSelectAreaLeftTop,
        SelectAreaRightTop = kTriglavPlugInPropertyPointDefaultValueKindSelectAreaRightTop,
        SelectAreaLeftBottom = kTriglavPlugInPropertyPointDefaultValueKindSelectAreaLeftBottom,
        SelectAreaRightBottom = kTriglavPlugInPropertyPointDefaultValueKindSelectAreaRightBottom,
        SelectAreaCenter = kTriglavPlugInPropertyPointDefaultValueKindSelectAreaCenter,
      };

    enum class PointMinMaxValueKinds : Int
      {
        Default = kTriglavPlugInPropertyPointMinMaxValueKindDefault,
        No = kTriglavPlugInPropertyPointMinMaxValueKindNo,
      };

    enum class CallBackNotifys : Int
      {
        ValueChanged = kTriglavPlugInPropertyCallBackNotifyValueChanged,
        ButtonPushed = kTriglavPlugInPropertyCallBackNotifyButtonPushed,
        ValueCheck = kTriglavPlugInPropertyCallBackNotifyValueCheck,
      };

    enum class CallBackResults : Int
      {
        NoModify = kTriglavPlugInPropertyCallBackResultNoModify,
        Modify = kTriglavPlugInPropertyCallBackResultModify,
        Invalid = kTriglavPlugInPropertyCallBackResultInvalid,
      };

    enum class ItemKey : Int {};

    constexpr static auto toItemKey( Int x ) noexcept -> ItemKey
      { return static_cast< ItemKey >( x ); }

    enum class ItemIndex : Int {};

    constexpr static auto toItemIndex( Int x ) noexcept -> ItemIndex
      { return static_cast< ItemIndex >( x ); }

    ~Property() = default;
    Property() = default;
    Property( Property const & ) = default;
    Property( Property && ) = default;
    auto operator =( Property const & ) -> Property & = default;
    auto operator =( Property && ) -> Property & = default;

    explicit Property( std::weak_ptr< Server const > const &server ) noexcept
      : ServiceBase{ server } {}

    auto service() const noexcept -> Service const *
      {
        if ( auto const psuite = suite() )
          { return psuite->propertyService; }
        return nullptr;
      }

    auto service2() const noexcept -> Service2 const *
      {
        if ( auto const psuite = suite() )
          { return psuite->propertyService2; }
        return nullptr;
    }

    auto make() -> APIResult< void >
      {
        APIResult< void > result;
        if ( auto const pservice = service() )
          {
            Object x{};
            auto const q = toAPIResults( pservice->createProc( &x ) );
            result.reset( q );
            if( result )
              { reset( *pservice, x, true ); }
          }
        return result;
      }

    auto addItem( ItemKey key, ValueTypes type, ValueKinds kind, InputKinds input, StringObject caption, Char accesskey ) const noexcept -> APIResult< void >
      {
        APIResult< void > result{};
        if ( auto const pservice = service() )
          {
            auto const a0 = static_cast< Int >( key );
            auto const a1 = static_cast< Int >( type );
            auto const a2 = static_cast< Int >( kind );
            auto const a3 = static_cast< Int >( input );
            auto const q = toAPIResults( pservice->addItemProc( *this, a0, a1, a2, a3, caption, accesskey ) );
            result.reset( q );
          }
        return result;
    }

    auto getBooleanValue( ItemKey key ) const noexcept -> APIResult< bool >
      {
        APIResult< bool > result{};
        if ( auto const pservice = service() )
          {
            Bool x{};
            auto const a = static_cast< Int >( key );
            auto const q = toAPIResults( pservice->getBooleanValueProc( &x, *this, a ) );
            result.reset( q, !!x );
          }
        return result;
      }

    auto setBooleanValue( ItemKey key, bool x ) const noexcept -> APIResult< void >
      {
        APIResult< void > result{};
        if ( auto const pservice = service() )
          {
            auto const a0 = static_cast< Int >( key );
            auto const a1 = x ? kBoolTrue : kBoolFalse;
            auto const q = toAPIResults( pservice->setBooleanValueProc( *this, a0, a1 ) );
            result.reset( q );
          }
        return result;
      }

    auto getBooleanDefaultValue( ItemKey key ) const noexcept -> APIResult< bool >
      {
        APIResult< bool > result{};
        if ( auto const pservice = service() )
          {
            Bool x{};
            auto const a = static_cast< Int >( key );
            auto const q = toAPIResults( pservice->getBooleanDefaultValueProc( &x, *this, a ) );
            result.reset( q, !!x );
          }
        return result;
      }

    auto setBooleanDefaultValue( ItemKey key, bool x ) const noexcept -> APIResult< void >
      {
        APIResult< void > result{};
        if ( auto const pservice = service() )
          {
            auto const a0 = static_cast< Int >( key );
            auto const a1 = x ? kBoolTrue : kBoolFalse;
            auto const q = toAPIResults( pservice->setBooleanDefaultValueProc( *this, a0, a1 ) );
            result.reset( q );
          }
        return result;
    }

    auto getIntegerValue( ItemKey key ) const noexcept -> APIResult< Integer >
      {
        APIResult< Integer > result{};
        if ( auto const pservice = service() )
          {
            Integer x{};
            auto const a = static_cast< Int >( key );
            auto const q = toAPIResults( pservice->getIntegerValueProc( &x, *this, a ) );
            result.reset( q, x );
          }
        return result;
      }

    auto setIntegerValue( ItemKey key, Integer x ) const noexcept -> APIResult< void >
      {
        APIResult< void > result{};
        if ( auto const pservice = service() )
          {
            auto const a = static_cast< Int >( key );
            auto const q = toAPIResults( pservice->setIntegerValueProc( *this, a, x ) );
            result.reset( q );
        }
        return result;
      }

    auto getIntegerDefaultValue( ItemKey key ) const noexcept -> APIResult< Integer >
      {
        APIResult< Integer > result{};
        if ( auto const pservice = service() )
          {
            Integer x{};
            auto const a = static_cast< Int >( key );
            auto const q = toAPIResults( pservice->getIntegerDefaultValueProc( &x, *this, a ) );
            result.reset( q, x );
          }
        return result;
      }

    auto setIntegerDefaultValue( ItemKey key, Integer x ) const noexcept -> APIResult< void >
      {
        APIResult< void > result{};
        if ( auto const pservice = service() )
          {
            auto const a = static_cast< Int >( key );
            auto const q = toAPIResults( pservice->setIntegerDefaultValueProc( *this, a, x ) );
            result.reset( q );
          }
        return result;
      }

    auto getIntegerMinValue( ItemKey key ) const noexcept -> APIResult< Integer >
      {
        APIResult< Integer > result{};
        if ( auto const pservice = service() )
          {
            Integer x{};
            auto const a = static_cast< Int >( key );
            auto const q = toAPIResults( pservice->getIntegerMinValueProc( &x, *this, a ) );
            result.reset( q, x );
          }
        return result;
      }

    auto setIntegerMinValue( ItemKey key, Integer x ) const noexcept -> APIResult< void >
      {
        APIResult< void > result{};
        if ( auto const pservice = service() )
          {
            auto const a = static_cast< Int >( key );
            auto const q = toAPIResults( pservice->setIntegerMinValueProc( *this, a, x ) );
            result.reset( q );
          }
        return result;
      }

    auto getIntegerMaxValue( ItemKey key ) const noexcept -> APIResult< Integer >
      {
        APIResult< Integer > result{};
        if ( auto const pservice = service() )
          {
            Integer x{};
            auto const a = static_cast< Int >( key );
            auto const q = toAPIResults( pservice->getIntegerMaxValueProc( &x, *this, a ) );
            result.reset( q, x );
          }
        return result;
      }

    auto setIntegerMaxValue( ItemKey key, Integer x ) const noexcept -> APIResult< void >
      {
        APIResult< void > result{};
        if ( auto const pservice = service() )
          {
            auto const a = static_cast< Int >( key );
            auto const q = toAPIResults( pservice->setIntegerMaxValueProc( *this, a, x ) );
            result.reset( q );
          }
        return result;
      }

    auto getDecimalValue( ItemKey key ) const noexcept -> APIResult< Decimal >
      {
        APIResult< Decimal > result{};
        if ( auto const pservice = service() )
          {
            Decimal x{};
            auto const a = static_cast< Int >( key );
            auto const q = toAPIResults( pservice->getDecimalValueProc( &x, *this, a ) );
            result.reset( q, x );
          }
        return result;
      }

    auto setDecimalValue( ItemKey key, Decimal x ) const noexcept -> APIResult< void >
      {
        APIResult< void > result{};
        if ( auto const pservice = service() )
          {
            auto const a = static_cast< Int >( key );
            auto const q = toAPIResults( pservice->setDecimalValueProc( *this, a, x ) );
            result.reset( q );
          }
        return result;
      }

    auto getDecimalDefaultValue( ItemKey key ) const noexcept -> APIResult< Decimal >
      {
        APIResult< Decimal > result{};
        if ( auto const pservice = service() )
          {
            Decimal x{};
            auto const a = static_cast< Int >( key );
            auto const q = toAPIResults( pservice->getDecimalDefaultValueProc( &x, *this, a ) );
            result.reset( q, x );
          }
        return result;
      }

    auto setDecimalDefaultValue( ItemKey key, Decimal x ) const noexcept -> APIResult< void >
      {
        APIResult< void > result{};
        if ( auto const pservice = service() )
          {
            auto const a = static_cast< Int >( key );
            auto const q = toAPIResults( pservice->setDecimalDefaultValueProc( *this, a, x ) );
            result.reset( q );
          }
        return result;
      }

    auto getDecimalMinValue( ItemKey key ) const noexcept -> APIResult< Decimal >
      {
        APIResult< Decimal > result{};
        if ( auto const pservice = service() )
          {
            Decimal x{};
            auto const a = static_cast< Int >( key );
            auto const q = toAPIResults( pservice->getDecimalMinValueProc( &x, *this, a ) );
            result.reset( q, x );
          }
        return result;
      }

    auto setDecimalMinValue( ItemKey key, Decimal x ) const noexcept -> APIResult< void >
      {
        APIResult< void > result{};
        if ( auto const pservice = service() )
          {
            auto const a = static_cast< Int >( key );
            auto const q = toAPIResults( pservice->setDecimalMinValueProc( *this, a, x ) );
            result.reset( q );
          }
        return result;
      }

    auto getDecimalMaxValue( ItemKey key ) const noexcept -> APIResult< Decimal >
      {
        APIResult< Decimal > result{};
        if ( auto const pservice = service() )
          {
            Decimal x{};
            auto const a = static_cast< Int >( key );
            auto const q = toAPIResults( pservice->getDecimalMaxValueProc( &x, *this, a ) );
            result.reset( q, x );
          }
        return result;
      }

    auto setDecimalMaxValue( ItemKey key, Decimal x ) const noexcept -> APIResult< void >
      {
        APIResult< void > result{};
        if ( auto const pservice = service() )
          {
            auto const a = static_cast< Int >( key );
            auto const q = toAPIResults( pservice->setDecimalMaxValueProc( *this, a, x ) );
            result.reset( q );
          }
        return result;
      }

    auto setItemStoreValue( ItemKey key, bool x ) const noexcept -> APIResult< void >
      {
        APIResult< void > result{};
        if ( auto const pservice = service2() )
          {
            auto const a0 = static_cast< Int >( key );
            auto const a1 = x ? kBoolTrue : kBoolFalse;
            auto const q = toAPIResults( pservice->setItemStoreValueProc( *this, a0, a1 ) );
            result.reset( q );
          }
        return result;
      }

    auto setPointValue( ItemKey key, Point const &x ) const noexcept -> APIResult< void >
      {
        APIResult< void > result{};
        if ( auto const pservice = service2() )
          {
            auto const a = static_cast< Int >( key );
            auto const q = toAPIResults( pservice->setPointValueProc( *this, a, &x ) );
            result.reset( q );
          }
        return result;
      }

    auto getPointValue( ItemKey key ) const noexcept -> APIResult< Point >
      {
        APIResult< Point > result{};
        if ( auto const pservice = service2() )
          {
            Point x{};
            auto const a = static_cast< Int >( key );
            auto const q = toAPIResults( pservice->getPointValueProc( &x, *this, a ) );
            result.reset( q, std::move( x ) );
          }
        return result;
      }

    auto setPointDefaultValueKind( ItemKey key, PointDefaultValueKinds x ) const noexcept -> APIResult< void >
      {
        APIResult< void > result{};
        if ( auto const pservice = service2() )
          {
            auto const a0 = static_cast< Int >( key );
            auto const a1 = static_cast< Int >( x );
            auto const q = toAPIResults( pservice->setPointDefaultValueKindProc( *this, a0, a1 ) );
            result.reset( q );
          }
        return result;
      }

    auto getPointDefaultValueKind( ItemKey key ) const noexcept -> APIResult< PointDefaultValueKinds >
      {
        APIResult< PointDefaultValueKinds > result{};
        if ( auto const pservice = service2() )
          {
            Int x{};
            auto const a = static_cast< Int >( key );
            auto const q = toAPIResults( pservice->getPointDefaultValueKindProc( &x, *this, a ) );
            result.reset( q, static_cast< PointDefaultValueKinds >( x ) );
          }
        return result;
      }

    auto setPointDefaultValue( ItemKey key, Point const &x ) const noexcept -> APIResult< void >
      {
        APIResult< void > result{};
        if ( auto const pservice = service2() )
          {
            auto const a = static_cast< Int >( key );
            auto const q = toAPIResults( pservice->setPointDefaultValueProc( *this, a, &x ) );
            result.reset( q );
          }
        return result;
      }

    auto getPointDefaultValue( ItemKey key ) const noexcept -> APIResult< Point >
      {
        APIResult< Point > result{};
        if ( auto const pservice = service2() )
          {
            Point x{};
            auto const a = static_cast< Int >( key );
            auto const q = toAPIResults( pservice->getPointDefaultValueProc( &x, *this, a ) );
            result.reset( q, std::move( x ) );
          }
        return result;
      }

    auto setPointMinMaxValueKind( ItemKey key, PointMinMaxValueKinds x ) const noexcept -> APIResult< void >
      {
        APIResult< void > result{};
        if ( auto const pservice = service2() )
          {
            auto const a0 = static_cast< Int >( key );
            auto const a1 = static_cast< Int >( x );
            auto const q = toAPIResults( pservice->setPointMinMaxValueKindProc( *this, a0, a1 ) );
            result.reset( q );
          }
        return result;
      }

    auto getPointMinMaxValueKind( ItemKey key ) const noexcept -> APIResult< PointMinMaxValueKinds >
      {
        APIResult< PointMinMaxValueKinds > result{};
        if ( auto const pservice = service2() )
          {
            Int x{};
            auto const a = static_cast< Int >( key );
            auto const q = toAPIResults( pservice->getPointMinMaxValueKindProc( &x, *this, a ) );
            result.reset( q, static_cast< PointMinMaxValueKinds >( x ) );
          }
        return result;
      }

    auto setPointMinValue( ItemKey key, Point const &x ) const noexcept -> APIResult< void >
      {
        APIResult< void > result{};
        if ( auto const pservice = service2() )
          {
            auto const a = static_cast< Int >( key );
            auto const q = toAPIResults( pservice->setPointMinValueProc( *this, a, &x ) );
            result.reset( q );
          }
        return result;
      }

    auto getPointMinValue( ItemKey key ) const noexcept -> APIResult< Point >
      {
        APIResult< Point > result{};
        if ( auto const pservice = service2() )
          {
            Point x{};
            auto const a = static_cast< Int >( key );
            auto const q = toAPIResults( pservice->getPointMinValueProc( &x, *this, a ) );
            result.reset( q, std::move( x ) );
          }
        return result;
      }

    auto setPointMaxValue( ItemKey key, Point const &x ) const noexcept -> APIResult< void >
      {
        APIResult< void > result{};
        if ( auto const pservice = service2() )
          {
            auto const a = static_cast< Int >( key );
            auto const q = toAPIResults( pservice->setPointMaxValueProc( *this, a, &x ) );
            result.reset( q );
          }
        return result;
      }

    auto getPointMaxValue( ItemKey key ) const noexcept -> APIResult< Point >
      {
        APIResult< Point > result{};
        if ( auto const pservice = service2() )
          {
            Point x{};
            auto const a = static_cast< Int >( key );
            auto const q = toAPIResults( pservice->getPointMaxValueProc( &x, *this, a ) );
            result.reset( q, std::move( x ) );
          }
        return result;
      }

    auto setEnumerationValue( ItemKey key, ItemIndex x ) const noexcept -> APIResult< void >
      {
        APIResult< void > result{};
        if ( auto const pservice = service2() )
          {
            auto const a0 = static_cast< Int >( key );
            auto const a1 = static_cast< Int >( x );
            auto const q = toAPIResults( pservice->setEnumerationValueProc( *this, a0, a1 ) );
            result.reset( q );
          }
        return result;
      }

    auto getEnumerationValue( ItemKey key ) const noexcept -> APIResult< ItemIndex >
      {
        APIResult< ItemIndex > result{};
        if ( auto const pservice = service2() )
          {
            Int x{};
            auto const a = static_cast< Int >( key );
            auto const q = toAPIResults( pservice->getEnumerationValueProc( &x, *this, a ) );
            result.reset( q, static_cast< ItemIndex >( x ) );
          }
        return result;
      }

    auto setEnumerationDefaultValue( ItemKey key, ItemIndex x ) const noexcept -> APIResult< void >
      {
        APIResult< void > result{};
        if ( auto const pservice = service2() )
          {
            auto const a0 = static_cast< Int >( key );
            auto const a1 = static_cast< Int >( x );
            auto const q = toAPIResults( pservice->setEnumerationDefaultValueProc( *this, a0, a1 ) );
            result.reset( q );
          }
        return result;
      }

    auto getEnumerationDefaultValue( ItemKey key ) const noexcept -> APIResult< ItemIndex >
      {
        APIResult< ItemIndex > result{};
        if ( auto const pservice = service2() )
          {
            Int x{};
            auto const a = static_cast< Int >( key );
            auto const q = toAPIResults( pservice->getEnumerationDefaultValueProc( &x, *this, a ) );
            result.reset( q, static_cast< ItemIndex >( x ) );
          }
        return result;
      }

    auto addEnumerationItem( ItemKey key, ItemIndex index, StringObject caption, Char accessKey ) const noexcept -> APIResult< void >
      {
        APIResult< void > result{};
        if ( auto const pservice = service2() )
          {
            auto const a0 = static_cast< Int >( key );
            auto const a1 = static_cast< Int >( index );
            auto const q = toAPIResults( pservice->addEnumerationItemProc( *this, a0, a1, caption, accessKey ) );
            result.reset( q );
          }
        return result;
      }

    auto setStringValue( ItemKey key, StringObject x ) const noexcept -> APIResult< void >
      {
        APIResult< void > result{};
        if ( auto const pservice = service2() )
          {
            auto const a = static_cast< Int >( key );
            auto const q = toAPIResults( pservice->setStringValueProc( *this, a, x ) );
            result.reset( q );
          }
        return result;
      }

    auto getStringValue( ItemKey key ) const noexcept -> APIResult< StringObject >
      {
        APIResult< StringObject > result{};
        if ( auto const pservice = service2() )
          {
            StringObject x{};
            auto const a = static_cast< Int >( key );
            auto const q = toAPIResults( pservice->getStringValueProc( &x, *this, a ) );
            result.reset( q, x );
          }
        return result;
      }

    auto setStringDefaultValue( ItemKey key, StringObject x ) const noexcept -> APIResult< void >
      {
        APIResult< void > result{};
        if ( auto const pservice = service2() )
          {
            auto const a = static_cast< Int >( key );
            auto const q = toAPIResults( pservice->setStringDefaultValueProc( *this, a, x ) );
            result.reset( q );
          }
        return result;
      }

    auto getStringDefaultValue( ItemKey key ) const noexcept -> APIResult< StringObject >
      {
        APIResult< StringObject > result{};
        if ( auto const pservice = service2() )
          {
            StringObject x{};
            auto const a = static_cast< Int >( key );
            auto const q = toAPIResults( pservice->getStringDefaultValueProc( &x, *this, a ) );
            result.reset( q, x );
          }
        return result;
      }

    auto setStringMaxLength( ItemKey key, Int x ) const noexcept -> APIResult< void >
      {
        APIResult< void > result{};
        if ( auto const pservice = service2() )
          {
            auto const a = static_cast< Int >( key );
            auto const q = toAPIResults( pservice->setStringMaxLengthProc( *this, a, x ) );
            result.reset( q );
          }
        return result;
      }

    auto getStringMaxLength( ItemKey key ) const noexcept -> APIResult< Int >
      {
        APIResult< Int > result{};
        if ( auto const pservice = service2() )
          {
            Int x{};
            auto const a = static_cast< Int >( key );
            auto const q = toAPIResults( pservice->getStringMaxLengthProc( &x, *this, a ) );
            result.reset( q, x );
          }
        return result;
      }
  };

inline auto makePropertyWithObject( std::weak_ptr< Server const > const &server, PropertyObject object, bool owned ) -> Property
  {
    Property result{ server };
    if ( auto const pservice = result.service() )
      { result.reset( *pservice, object, owned ); }
    return result;
}

inline auto makeProperty( std::weak_ptr< Server const > const &server ) -> Property
  {
    Property result{ server };
    result.make();
    return result;
  }


class ModuleInitializer : public RecordBase
  {
  public:
    using Record = ModuleInitializeRecord;

    enum class ModuleKinds : Int
      {
        Filter = kTriglavPlugInModuleKindFilter,
        FilterActivation = kTriglavPlugInModuleKindFilterActivation,
      };

    constexpr static Int kNeedHostVersion = kTriglavPlugInNeedHostVersion;
    constexpr static ModuleKinds kSwitchKindFilter = static_cast< ModuleKinds >( kTriglavPlugInModuleSwitchKindFilter );

    ~ModuleInitializer() = default;
    ModuleInitializer() = default;
    ModuleInitializer( ModuleInitializer const & ) = default;
    ModuleInitializer( ModuleInitializer && ) = default;
    auto operator =( ModuleInitializer const & ) -> ModuleInitializer & = default;
    auto operator =( ModuleInitializer && ) -> ModuleInitializer & = default;

      explicit ModuleInitializer( std::weak_ptr< Server const > const &server ) noexcept
      : RecordBase{ server } {}

    auto record() const noexcept -> Record const *
      {
        if ( auto const psuite = suite() )
          { return psuite->moduleInitializeRecord; }
        return nullptr;
      }

    auto getHostVersion() const noexcept -> APIResult< Int >
      {
        APIResult< Int > result{};
        if ( auto const precord = record() )
          {
            Int x {};
            auto const a = host();
            auto const q = toAPIResults( precord->getHostVersionProc( &x, a ) );
            result.reset( q, x );
          }
        return result;
      }

    auto setModuleId( StringObject x ) const noexcept -> APIResult< void >
      {
        APIResult< void > result{};
        if ( auto const precord = record() )
          {
            auto const a = host();
            auto const q = toAPIResults( precord->setModuleIDProc( a, x ) );
            result.reset( q );
          }
        return result;
      }

    auto setModuleKind( ModuleKinds x ) const noexcept -> APIResult< void >
      {
        APIResult< void > result{};
        if ( auto const precord = record() )
          {
            auto const a0 = host();
            auto const a1 = static_cast< Int >( x );
            auto const q = toAPIResults( precord->setModuleKindProc( a0, a1 ) );
            result.reset( q );
          }
        return result;
      }
  };

inline auto makeModuleInitializer( std::weak_ptr< Server const > const & server ) noexcept -> ModuleInitializer
  { return ModuleInitializer {server}; }


class FilterInitializer : public RecordBase
  {
  public:
    enum class TargetKinds : Int
      {
        RasterLayerGrayAlpha = kTriglavPlugInFilterTargetKindRasterLayerGrayAlpha,
        RasterLayerRGBAlpha = kTriglavPlugInFilterTargetKindRasterLayerRGBAlpha,
        RasterLayerCMYKAlpha = kTriglavPlugInFilterTargetKindRasterLayerCMYKAlpha,
        RasterLayerAlpha = kTriglavPlugInFilterTargetKindRasterLayerAlpha,
        RasterLayerBinarizationAlpha = kTriglavPlugInFilterTargetKindRasterLayerBinarizationAlpha,
        RasterLayerBinarizationGrayAlpha = kTriglavPlugInFilterTargetKindRasterLayerBinarizationGrayAlpha,
      };

    ~FilterInitializer() = default;
    FilterInitializer() = default;
    FilterInitializer( FilterInitializer const & ) = default;
    FilterInitializer( FilterInitializer && ) = default;
    auto operator=( FilterInitializer const & ) -> FilterInitializer & = default;
    auto operator=( FilterInitializer && ) -> FilterInitializer & = default;

    explicit FilterInitializer( std::weak_ptr< Server const > const &server ) noexcept
      : RecordBase{ server } {}

    auto setFilterCategoryName( StringObject filterCategoryName, Char accessKey ) const noexcept -> APIResult< void >
      {
        APIResult< void > result{};
        if ( auto const psuite = suite() )
          {
            auto const a = host();
            auto const q = toAPIResults( TriglavPlugInFilterInitializeSetFilterCategoryName( psuite, a, filterCategoryName, accessKey ) );
            result.reset( q );
          }
        return result;
      }

    auto setFilterName( StringObject filterName, Char accessKey ) const noexcept -> APIResult< void >
      {
        APIResult< void > result{};
        if ( auto const psuite = suite() )
          {
            auto const a = host();
            auto const q = toAPIResults( TriglavPlugInFilterInitializeSetFilterName( psuite, a, filterName, accessKey ) );
            result.reset( q );
          }
        return result;
      }

    auto setCanPreview( bool x ) const noexcept -> APIResult< void >
      {
        APIResult< void > result{};
        if ( auto const psuite = suite() )
          {
            auto const a0 = host();
            auto const a1 = x ? kBoolTrue : kBoolFalse;
            auto const q = toAPIResults( TriglavPlugInFilterInitializeSetCanPreview( psuite, a0, a1 ) );
            result.reset( q );
          }
        return result;
      }

    auto setUseBlankImage( bool x ) const noexcept -> APIResult< void >
      {
        APIResult< void > result{};
        if ( auto const psuite = suite() )
          {
            auto const a0 = host();
            auto const a1 = x ? kBoolTrue : kBoolFalse;
            auto const q = toAPIResults( TriglavPlugInFilterInitializeSetUseBlankImage( psuite, a0, a1 ) );
            result.reset( q );
          }
        return result;
      }

    auto setTargetKinds( TargetKinds const *targetKinds, Int targetKindCount ) const noexcept -> APIResult< void >
      {
        APIResult< void > result{};
        if ( auto const psuite = suite() )
          {
            auto const a0 = host();
            auto const a1 = reinterpret_cast< Int const * >( targetKinds );
            auto const q = toAPIResults( TriglavPlugInFilterInitializeSetTargetKinds( psuite, a0, a1, targetKindCount ) );
            result.reset( q );
          }
        return result;
    }

    auto setProperty( Property::Object x ) const noexcept -> APIResult< void >
      {
        APIResult< void > result{};
        if ( auto const psuite = suite() )
          {
            auto const a = host();
            auto const q = toAPIResults( TriglavPlugInFilterInitializeSetProperty( psuite, a, x ) );
            result.reset( q );
          }
        return result;
      }

    auto setPropertyCallBack( Property::CallBackProc callBackProc, Ptr data ) const noexcept -> APIResult< void >
      {
        APIResult< void > result{};
        if ( auto const psuite = suite() )
          {
            auto const a = host();
            auto const q = toAPIResults( TriglavPlugInFilterInitializeSetPropertyCallBack( psuite, a, callBackProc, data ) );
            result.reset( q );
          }
        return result;
      }
  };

inline auto makeFilterInitializer( std::weak_ptr< Server const > const &server ) noexcept -> FilterInitializer
  { return FilterInitializer{ server }; }


class FilterRunner : public RecordBase
  {
  public:
    enum class ProcessStates : Int
      {
        Start = kTriglavPlugInFilterRunProcessStateStart,
        Continue = kTriglavPlugInFilterRunProcessStateContinue,
        End = kTriglavPlugInFilterRunProcessStateEnd,
        Abort = kTriglavPlugInFilterRunProcessStateAbort,
      };

    enum class ProcessResults : Int
      {
        Continue = kTriglavPlugInFilterRunProcessResultContinue,
        Restart = kTriglavPlugInFilterRunProcessResultRestart,
        Exit = kTriglavPlugInFilterRunProcessResultExit,
      };

    ~FilterRunner() = default;
    FilterRunner() = default;
    FilterRunner( FilterRunner const & ) = default;
    FilterRunner( FilterRunner && ) = default;
    auto operator =( FilterRunner const & ) -> FilterRunner & = default;
    auto operator =( FilterRunner && ) -> FilterRunner & = default;

    explicit FilterRunner( std::weak_ptr< Server const > const &server ) noexcept
      : RecordBase( server ) {}

    auto getProperty() const noexcept -> APIResult< PropertyObject >
      {
        APIResult< PropertyObject > result{};
        if ( auto const psuite = suite() )
          {
            PropertyObject x{};
            auto const a = host();
            auto const q = toAPIResults( TriglavPlugInFilterRunGetProperty( psuite, &x, a ) );
            result.reset( q, x );
          }
        return result;
      }

    auto getCanvasWidth() const noexcept -> APIResult< Int >
      {
        APIResult< Int > result{};
        if ( auto const psuite = suite() )
          {
            Int x{};
            auto const a = host();
            auto const q = toAPIResults( TriglavPlugInFilterRunGetCanvasWidth( psuite, &x, a ) );
            result.reset( q, x );
          }
        return result;
      }

    auto getCanvasHeight() const noexcept -> APIResult< Int >
      {
        APIResult< Int > result{};
        if ( auto const psuite = suite() )
          {
            Int x{};
            auto const a = host();
            auto const q = toAPIResults( TriglavPlugInFilterRunGetCanvasHeight( psuite, &x, a ) );
            result.reset( q, x );
          }
        return result;
      }

    auto getCanvasResolution() const noexcept -> APIResult< Double >
      {
        APIResult< Double > result{};
        if ( auto const psuite = suite() )
          {
            Double x{};
            auto const a = host();
            auto const q = toAPIResults( TriglavPlugInFilterRunGetCanvasResolution( psuite, &x, a ) );
            result.reset( q, x );
          }
        return result;
      }

    auto getLayerOrigin() const noexcept -> APIResult< Point >
      {
        APIResult< Point > result{};
        if ( auto const psuite = suite() )
          {
            Point x{};
            auto const a = host();
            auto const q = toAPIResults( TriglavPlugInFilterRunGetLayerOrigin( psuite, &x, a ) );
            result.reset( q, std::move( x ) );
          }
        return result;
      }

    auto isLayerMaskSelected() const noexcept -> APIResult< bool >
      {
        APIResult< bool > result{};
        if ( auto const psuite = suite() )
          {
            Bool x{};
            auto const a = host();
            auto const q = toAPIResults( TriglavPlugInFilterRunIsLayerMaskSelected( psuite, &x, a ) );
            result.reset( q, !!x );
          }
        return result;
      }

    auto isAlphaLocked() const noexcept -> APIResult< bool >
      {
        APIResult< bool > result;
        if ( auto const psuite = suite() )
          {
            Bool x{};
            auto const a = host();
            auto const q = toAPIResults( TriglavPlugInFilterRunIsAlphaLocked( psuite, &x, a ) );
            result.reset( q, !!x );
          }
        return result;
      }

    auto getSourceOffscreen() const noexcept -> APIResult< OffscreenObject >
      {
        APIResult< OffscreenObject > result{};
        if ( auto const psuite = suite() )
          {
            OffscreenObject x{};
            auto const a = host();
            auto const q = toAPIResults( TriglavPlugInFilterRunGetSourceOffscreen( psuite, &x, a ) );
            result.reset( q, x );
          }
        return result;
      }

    auto getDestinationOffscreen() const noexcept -> APIResult< OffscreenObject >
      {
        APIResult< OffscreenObject > result{};
        if ( auto const psuite = suite() )
          {
            OffscreenObject x{};
            auto const a = host();
            auto const q = toAPIResults( TriglavPlugInFilterRunGetDestinationOffscreen( psuite, &x, a ) );
            result.reset( q, x );
          }
        return result;
      }

    auto hasSelectAreaOffscreen() const noexcept -> APIResult< bool >
      {
        APIResult< bool > result{};
        if ( auto const psuite = suite() )
          {
            Bool x{};
            auto const a = host();
            auto const q = toAPIResults( TriglavPlugInFilterRunHasSelectAreaOffscreen( psuite, &x, a ) );
            result.reset( q, !!x );
          }
        return result;
      }

    auto getSelectAreaRect() const noexcept -> APIResult< Rect >
      {
        APIResult< Rect > result{};
        if ( auto const psuite = suite() )
          {
            Rect x{};
            auto const a = host();
            auto const q = toAPIResults( TriglavPlugInFilterRunGetSelectAreaRect( psuite, &x, a ) );
            result.reset( q, std::move( x ) );
          }
        return result;
      }

    auto getSelectAreaOffscreen() const noexcept -> APIResult< OffscreenObject >
      {
        APIResult< OffscreenObject > result{};
        if ( auto const psuite = suite() )
          {
            OffscreenObject x{};
            auto const a = host();
            auto const q = toAPIResults( TriglavPlugInFilterRunGetSelectAreaOffscreen( psuite, &x, a ) );
            result.reset( q, x );
          }
        return result;
      }

    auto updateDestinationOffscreenRect( Rect const &x ) const noexcept -> APIResult< void >
      {
        APIResult< void > result{};
        if ( auto const psuite = suite() )
          {
            auto const a = host();
            auto const q = toAPIResults( TriglavPlugInFilterRunUpdateDestinationOffscreenRect( psuite, a, &x ) );
            result.reset( q );
          }
        return result;
      }

    auto getMainColor() const noexcept -> APIResult< std::tuple< RGBColor, UInt8 > >
      {
        APIResult< std::tuple< RGBColor, UInt8 > > result{};
        if ( auto const psuite = suite() )
          {
            RGBColor x0{};
            UInt8 x1{};
            auto const a = host();
            auto const q = toAPIResults( TriglavPlugInFilterRunGetMainColor( psuite, &x0, &x1, a ) );
            result.reset( q, std::make_tuple( std::move( x0 ), x1 ) );
          }
          return result;
      }

    auto getSubColor() const noexcept -> APIResult< std::tuple< RGBColor, UInt8 > >
      {
        APIResult< std::tuple< RGBColor, UInt8 > > result{};
        if ( auto const psuite = suite() )
          {
            RGBColor x0{};
            UInt8 x1{};
            auto const a = host();
            auto const q = toAPIResults( TriglavPlugInFilterRunGetSubColor( psuite, &x0, &x1, a ) );
            result.reset( q, std::make_tuple( std::move( x0 ), x1 ) );
          }
          return result;
      }

    auto getDrawColor() const noexcept -> APIResult< std::tuple< RGBColor, UInt8 > >
      {
        APIResult< std::tuple< RGBColor, UInt8 > > result{};
        if ( auto const psuite = suite() )
          {
            RGBColor x0{};
            UInt8 x1{};
            auto const a = host();
            auto const q = toAPIResults( TriglavPlugInFilterRunGetDrawColor( psuite, &x0, &x1, a ) );
            result.reset( q, std::make_tuple( std::move( x0 ), x1 ) );
          }
          return result;
      }

    auto process( ProcessStates state ) const noexcept -> APIResult< ProcessResults >
      {
        APIResult< ProcessResults > result{};
        if ( auto const psuite = suite() )
          {
            Int x{};
            auto const a0 = host();
            auto const a1 = static_cast< Int >( state );
            auto const q = toAPIResults( TriglavPlugInFilterRunProcess( psuite, &x, a0, a1 ) );
            result.reset( q, static_cast< ProcessResults >( x ) );
          }
        return result;
      }

    auto setProgressTotal( Int x ) const noexcept -> APIResult< void >
      {
        APIResult< void > result{};
        if ( auto const psuite = suite() )
          {
            auto const a = host();
            auto const q = toAPIResults( TriglavPlugInFilterRunSetProgressTotal( psuite, a, x ) );
            result.reset( q );
          }
        return result;
    }

    auto setProgressDone( Int x ) const noexcept -> APIResult< void >
      {
        APIResult< void > result{};
        if ( auto const psuite = suite() )
          {
            auto const a = host();
            auto const q = toAPIResults( TriglavPlugInFilterRunSetProgressDone( psuite, a, x ) );
            result.reset( q );
          }
        return result;
      }
  };

inline auto makeFilterRunner( std::weak_ptr< Server const > const &server ) noexcept -> FilterRunner
  { return FilterRunner{ server }; }

}} // namespace Triglav::PlugIn

#endif // cspsdkxx_triglavpluginsdk_trigravpluginsdk_hh_
