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

        auto const selectAreaRect = *fr.getSelectAreaRect();
        auto const destinationOffscreen = makeOffscreenWithObject( server_, *fr.getDestinationOffscreen(), false );

        auto const count = *destinationOffscreen.getBlockRectCount( selectAreaRect );
        fr.setProgressTotal( count );

        using S = FilterRunner::ProcessStates;
        using R = FilterRunner::ProcessResults;
        auto index = decltype( count ){};
        for ( auto result = R::Restart; result != R::Exit; result = *fr.process( index < count ? !index ? S::Start : S::Continue : S::End ) )
          {
            if ( result == R::Restart )
              {
                // todo

                index = 0;
                continue;
              }

            auto const rect = *destinationOffscreen.getBlockRect( index, selectAreaRect );

            // todo

            fr.updateDestinationOffscreenRect( rect );
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
                break;
            case Property::CallBackNotifys::ValueChanged:
                cbResult = self->onValueChanged( prop, k );
                break;
          }

        *result = static_cast< Int >( cbResult );
      }

    auto onButtonPushed( Property const &prop, Property::ItemKey itemKey ) noexcept -> Property::CallBackResults
      {
        // todo
        return Property::CallBackResults::Modify;
      }

    auto onValueChanged( Property const &prop, Property::ItemKey itemKey ) noexcept -> Property::CallBackResults
      {
        // todo
        return Property::CallBackResults::Modify;
      }

    std::weak_ptr< Server const > server_;
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
