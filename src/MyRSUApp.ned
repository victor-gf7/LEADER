package veins_manhattan.src;
import org.car2x.veins.modules.application.ieee80211p.BaseWaveApplLayer;


simple MyRSUApp extends BaseWaveApplLayer
{
    @class(MyRSUApp);
    @display("i=block/app");
    double windowTime = default(15.0);
}
