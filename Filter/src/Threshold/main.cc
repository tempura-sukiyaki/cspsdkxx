#include <algorithm>
#include <memory>
#include <utility>
#include <vector>

#include <TriglavPlugInSDK/TriglavPlugInSDK.hh>

namespace Triglav { namespace PlugIn {

namespace {

// ModuleId
String::Data const kModuleId{ u"6904699D-BAF9-4669-893F-0C1F29EB88B3" };

// CategoryName
constexpr auto kCategoryName = toStringId( 100 );
constexpr auto kCategoryAccessKey = toStringId( 101 );

// Name
constexpr auto kName = toStringId( 200 );
constexpr auto kAccessKey = toStringId( 201 );

// TargetKinds
std::vector< FilterInitializer::TargetKinds > const kTargetKinds{
  FilterInitializer::TargetKinds::RasterLayerAlpha,
  FilterInitializer::TargetKinds::RasterLayerGrayAlpha,
  FilterInitializer::TargetKinds::RasterLayerRGBAlpha,
};

// UseBlankImage
constexpr auto kUseBlankImage = false;

// CanPreview
constexpr auto kCanPreview = true;

// Threshold
constexpr auto kThresholdName = toStringId( 10000 );
constexpr auto kThresholdAccessKey = toStringId( 10001 );
constexpr auto kThresholdItemKey = Property::toItemKey( 10000 );
constexpr auto kThresholdMinValue = 1;
constexpr auto kThresholdMaxValue = 255;
constexpr auto kThresholdDefaultValue = ( kThresholdMinValue + kThresholdMaxValue ) / 2;
constexpr auto kThresholdStoreValue = false;

auto makeCaption( std::weak_ptr< Server const > const &server, String::Data const &name, String::Data const &accessKey ) noexcept -> APIResult< std::pair< String, Char > >
  {
    if ( auto const strName = makeStringWithData( server, name ) )
      {
        if ( auto const strAccessKey = makeStringWithData( server, accessKey ) )
          {
            if ( auto const str = strAccessKey.getLocalCodeString() )
              {
                if ( !str->empty() )
                  { return std::make_pair( strName, ( *str )[ 0 ] ); }
              }
          }
      }
    return {};
  }

auto lerp( UInt8 a, UInt8 b, UInt8 c ) noexcept -> UInt8
  { return static_cast< UInt8 >( ( a * ( 0xFF - c ) + b * c + 0x7F ) / 0xFF ); }

class Filter
  {
  public:
    ~Filter() = default;
    Filter() = default;
    Filter( Filter const & ) = delete;
    Filter( Filter && ) = default;
    auto operator =( Filter const & ) -> Filter & = delete;
    auto operator =( Filter && ) -> Filter & = default;

    auto moduleInitialize( std::weak_ptr< Server const > const &server ) noexcept -> CallResults
      {
        server_ = server;

        auto const mi = makeModuleInitializer( server_ );

        // HostVersion
        if ( auto const hostVersion = mi.getHostVersion() )
          {
            if ( *hostVersion < ModuleInitializer::kNeedHostVersion )
              { return CallResults::Failed; }
          }
        else
          { return CallResults::Failed; }

        // ModuleKind
        if ( !mi.setModuleKind( ModuleInitializer::kSwitchKindFilter ) )
          { return CallResults::Failed; }

        // ModuleId
        if ( auto const moduleId = makeStringWithData( server_, kModuleId ) )
          {
            if ( !mi.setModuleId( moduleId ) )
              { return CallResults::Failed; }
          }
        else
          { return CallResults::Failed; }

        return CallResults::Success;
      }

    auto initialize( std::weak_ptr< Server const > const &server ) noexcept -> CallResults
      {
        server_ = server;

        auto const fi = makeFilterInitializer( server_ );

        // CategoryName
        if ( auto const pair = makeCaption( server_, kCategoryName, kCategoryAccessKey ) )
          {
            if ( !fi.setFilterCategoryName( pair->first, pair->second ) )
              { return CallResults::Failed; }
          }
        else
          { return CallResults::Failed; }

        // Name
        if ( auto const pair = makeCaption( server_, kName, kAccessKey ) )
          {
            if ( !fi.setFilterName( pair->first, pair->second ) )
              { return CallResults::Failed; }
          }
        else
          { return CallResults::Failed; }

        // TargetKinds
        if ( !fi.setTargetKinds( kTargetKinds.data(), static_cast< Int >( kTargetKinds.size() ) ) )
          { return CallResults::Failed; }

        // UseBlankImage
        if ( !fi.setUseBlankImage( kUseBlankImage ) )
          { return CallResults::Failed; }

        if ( auto const prop = makeProperty( server_ ) )
          {
            // CanPreview
            if ( !fi.setCanPreview( kCanPreview ) )
              { return CallResults::Failed; }

            // Threshold
            if ( auto pair = makeCaption( server_, kThresholdName, kThresholdAccessKey ) )
              {
                if ( !prop.addItem( kThresholdItemKey, Property::ValueTypes::Integer, Property::ValueKinds::Default, Property::InputKinds::Default, pair->first, pair->second ) )
                  { return CallResults::Failed; }
                if ( !prop.setIntegerMinValue( kThresholdItemKey, kThresholdMinValue ) )
                  { return CallResults::Failed; }
                if ( !prop.setIntegerMaxValue( kThresholdItemKey, kThresholdMaxValue ) )
                  { return CallResults::Failed; }
                if ( !prop.setIntegerDefaultValue( kThresholdItemKey, kThresholdDefaultValue ) )
                  { return CallResults::Failed; }
                if ( !prop.setItemStoreValue( kThresholdItemKey, kThresholdStoreValue ) )
                  { return CallResults::Failed; }
              }
            else
              { return CallResults::Failed; }

            // PropertyCallBack
            if ( !fi.setPropertyCallBack( &propertyCallBack, this ) )
              { return CallResults::Failed; }

            // Property
            if ( !fi.setProperty( prop ) )
              { return CallResults::Failed; }
          }
        else
          { return CallResults::Failed; }

        return CallResults::Success;
      }

    auto run( std::weak_ptr< Server const > const &server ) noexcept -> CallResults
      {
        server_ = server;

        auto const fr = makeFilterRunner( server_ );

        auto const destinationOffscreen = makeOffscreenWithObject( server_, *fr.getDestinationOffscreen(), false );

        std::vector< Int > channelIndexs{};
        switch ( *destinationOffscreen.getChannelOrder() )
          {
            case Offscreen::ChannelOrders::Alpha:
              if ( std::find( kTargetKinds.begin(), kTargetKinds.end(), FilterInitializer::TargetKinds::RasterLayerAlpha ) == kTargetKinds.end() )
                { return CallResults::Failed; }
              break;

            case Offscreen::ChannelOrders::GrayAlpha:
              if ( std::find( kTargetKinds.begin(), kTargetKinds.end(), FilterInitializer::TargetKinds::RasterLayerGrayAlpha ) == kTargetKinds.end() )
                { return CallResults::Failed; }
              channelIndexs.push_back( 0 );
              break;

            case Offscreen::ChannelOrders::RGBAlpha:
              if ( std::find( kTargetKinds.begin(), kTargetKinds.end(), FilterInitializer::TargetKinds::RasterLayerRGBAlpha ) == kTargetKinds.end() )
                { return CallResults::Failed; }
              if ( auto const indexs = destinationOffscreen.getRGBChannelIndex() )
                {
                  channelIndexs.push_back( std::get< 0 >( *indexs ) );
                  channelIndexs.push_back( std::get< 1 >( *indexs ) );
                  channelIndexs.push_back( std::get< 2 >( *indexs ) );
                }
              else
                { return CallResults::Failed; }
              break;

            case Offscreen::ChannelOrders::CMYKAlpha:
              if ( std::find( kTargetKinds.begin(), kTargetKinds.end(), FilterInitializer::TargetKinds::RasterLayerCMYKAlpha ) == kTargetKinds.end() )
                { return CallResults::Failed; }
              if ( auto const indexs = destinationOffscreen.getCMYKChannelIndex() )
                {
                  channelIndexs.push_back( std::get< 0 >( *indexs ) );
                  channelIndexs.push_back( std::get< 1 >( *indexs ) );
                  channelIndexs.push_back( std::get< 2 >( *indexs ) );
                  channelIndexs.push_back( std::get< 3 >( *indexs ) );
                }
              else
                { return CallResults::Failed; }
              break;

            case Offscreen::ChannelOrders::BinarizationAlpha:
              if ( std::find( kTargetKinds.begin(), kTargetKinds.end(), FilterInitializer::TargetKinds::RasterLayerBinarizationAlpha ) == kTargetKinds.end() )
                { return CallResults::Failed; }
              break;

            case Offscreen::ChannelOrders::BinarizationGrayAlpha:
              if ( std::find( kTargetKinds.begin(), kTargetKinds.end(), FilterInitializer::TargetKinds::RasterLayerBinarizationGrayAlpha ) == kTargetKinds.end() )
                { return CallResults::Failed; }
              channelIndexs.push_back( 0 );
              break;

            case Offscreen::ChannelOrders::SelectArea:
            case Offscreen::ChannelOrders::Plane:
              return CallResults::Failed;
          }

        auto const selectAreaRect = *fr.getSelectAreaRect();

        auto const count = *destinationOffscreen.getBlockRectCount( selectAreaRect );
        fr.setProgressTotal( count );

        using S = FilterRunner::ProcessStates;
        using R = FilterRunner::ProcessResults;
        auto index = decltype( count ){};
        for ( auto result = R::Restart; result != R::Exit; result = *fr.process( index < count ? !index ? S::Start : S::Continue : S::End ) )
          {
            if ( result == R::Restart )
              {
                index = 0;
                continue;
              }

            auto const blockRect = *destinationOffscreen.getBlockRect( index, selectAreaRect );
            Point const blockPos{ blockRect.left, blockRect.top };

            if ( auto const blockAlpha = destinationOffscreen.getMutableBlockAlpha( blockPos ) )
              {
                static Byte const one = toByte( 0xFF );
                APIResult< Offscreen::Block > blockSelectArea = Offscreen::Block{ &one, 0, 0, blockAlpha->rect };
                if ( *fr.hasSelectAreaOffscreen() )
                  {
                    auto const selectAreaOffscreen = makeOffscreenWithObject( server_, *fr.getSelectAreaOffscreen(), false );
                    blockSelectArea = selectAreaOffscreen.getBlockSelectArea( blockPos );
                  }
                if ( blockSelectArea )
                  {
                    if ( channelIndexs.empty() )
                      { executeBlock( blockRect, *blockSelectArea, *blockAlpha ); }
                    else if ( auto const blockImage = destinationOffscreen.getMutableBlockImage( blockPos )
                         )
                      { executeBlock( blockRect, *blockSelectArea, *blockAlpha, *blockImage, channelIndexs ); }
                  }
              }

            fr.updateDestinationOffscreenRect( blockRect );
            ++index;
            fr.setProgressDone( index );
          }

        return CallResults::Success;
      }

    auto terminate( std::weak_ptr< Server const > const &server ) noexcept -> CallResults
      {
        server_ = server;
        return CallResults::Success;
      }

    auto moduleTerminate( std::weak_ptr< Server const > const &server ) noexcept -> CallResults
      {
        server_ = server;
        return CallResults::Success;
      }

  private:
    static void TP_CALLBACK propertyCallBack( Int *result, Property::Object object, Int itemKey, Int notify, Ptr data ) noexcept
      {
        auto self = static_cast< Filter * >( data );
        auto cbResult = Property::CallBackResults::Invalid;

        auto const prop = makePropertyWithObject( self->server_, object, false );

        auto const k = Property::toItemKey( itemKey );
        switch ( static_cast< Property::CallBackNotifys >( notify ) )
          {
            case Property::CallBackNotifys::ButtonPushed:
                cbResult = self->onButtonPushed( prop, k );
                break;
            case Property::CallBackNotifys::ValueCheck:
                cbResult = static_cast< Property::CallBackResults >( *result );
                break;
            case Property::CallBackNotifys::ValueChanged:
                cbResult = self->onValueChanged( prop, k );
                break;
          }

        *result = static_cast< Int >( cbResult );
      }

    auto onButtonPushed( Property const &prop, Property::ItemKey itemKey ) noexcept -> Property::CallBackResults
      {
        switch ( itemKey )
          {
            default:
              break;
          }
        return Property::CallBackResults::NoModify;
      }

    auto onValueChanged( Property const &prop, Property::ItemKey itemKey ) noexcept -> Property::CallBackResults
      {
        switch ( itemKey )
          {
            case kThresholdItemKey:
              if ( auto const threshold = prop.getIntegerValue( itemKey ) )
                {
                  if ( threshold_ != *threshold )
                    {
                      threshold_ = *threshold;
                      return Property::CallBackResults::Modify;
                    }
                }
              break;

            default:
              break;
          }
        return Property::CallBackResults::NoModify;
      }

    void executeBlock( Rect const &blockRect, Offscreen::Block const &blockSelectArea, Offscreen::MutableBlock const &blockAlpha ) noexcept
      {
        for ( auto y = blockRect.top; y < blockRect.bottom; ++y )
          {
            auto selectPtr = reinterpret_cast< UInt8 const * >( blockSelectArea.address + ( blockSelectArea.rowBytes * ( y - blockRect.top ) ) );
            auto alphaPtr = reinterpret_cast< UInt8 * >( blockAlpha.address + ( blockAlpha.rowBytes * ( y - blockRect.top ) ) );
            for ( auto x = blockRect.left; x < blockRect.right; ++x )
              {
                UInt8 const ch = *alphaPtr < threshold_ ? 0x00 : 0xFF;
                if ( *selectPtr == 0xFF )
                  { *alphaPtr = ch; }
                else if ( *selectPtr )
                  { *alphaPtr = lerp( *alphaPtr, ch, *selectPtr ); }
                selectPtr += blockSelectArea.pixelBytes;
                alphaPtr += blockAlpha.pixelBytes;
              }
          }
      }

    void executeBlock( Rect const &blockRect, Offscreen::Block const &blockSelectArea, Offscreen::MutableBlock const &blockAlpha, Offscreen::MutableBlock const &blockImage, std::vector< Int > const &channelIndexs ) noexcept
      {
        for ( auto y = blockRect.top; y < blockRect.bottom; ++y )
          {
            auto selectPtr = reinterpret_cast< UInt8 const * >( blockSelectArea.address + ( blockSelectArea.rowBytes * ( y - blockRect.top ) ) );
            auto alphaPtr = reinterpret_cast< UInt8 * >( blockAlpha.address + ( blockAlpha.rowBytes * ( y - blockRect.top ) ) );
            auto imagePtr = reinterpret_cast< UInt8 * >( blockImage.address + ( blockImage.rowBytes * ( y - blockRect.top ) ) );
            for ( auto x = blockRect.left; x < blockRect.right; ++x )
              {
                for ( auto &&i : channelIndexs )
                  {
                    UInt8 const ch = imagePtr[ i ] < threshold_ ? 0x00 : 0xFF;
                    if ( *selectPtr == 0xFF )
                      { imagePtr[ i ] = ch; }
                    else if ( *selectPtr )
                      { imagePtr[ i ] = lerp( imagePtr[ i ], ch, *selectPtr ); }
                  }
                selectPtr += blockSelectArea.pixelBytes;
                alphaPtr += blockAlpha.pixelBytes;
                imagePtr += blockImage.pixelBytes;
              }
          }
      }

    std::weak_ptr< Server const > server_;
    Integer threshold_;
  };

} // namespace

}} // namespace Triglav::PlugIn

using namespace Triglav::PlugIn;

void TP_CALLBACK TriglavPluginCall( Int *result, Ptr *data, Int selector, Server *server, Ptr reserved )
  {
    auto callResult = CallResults::Failed;

    std::shared_ptr< Server const > server_{ server, []( auto ) {} };

    auto filter = static_cast< Filter * >( *data );

    switch ( static_cast< Selectors >( selector ) )
      {
        case Selectors::ModuleInitialize:
          *data = filter = new ( std::nothrow ) Filter{};
          if ( filter )
            { callResult = filter->moduleInitialize( server_ ); }
          break;

        case Selectors::FilterInitialize:
          callResult = filter->initialize( server_ );
          break;

        case Selectors::FilterRun:
          callResult = filter->run( server_ );
          break;

        case Selectors::FilterTerminate:
          callResult = filter->terminate( server_ );
          break;

        case Selectors::ModuleTerminate:
          callResult = filter->moduleTerminate( server_ );
          delete filter;
          *data = filter = nullptr;
          break;
      }

    *result = static_cast< Int >( callResult );
  }
