
package veins_manhattan.src;

import org.car2x.veins.modules.application.ieee80211p.BaseWaveApplLayer;

simple MyApplicaton extends BaseWaveApplLayer
{
    //declarations go here
    @class(MyApplicaton);
    @display("i=block/segm");
    double resumptionWindowTime = default(10.0);
}
