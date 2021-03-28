
package veins_manhattan.src;

import org.car2x.veins.base.connectionManager.ConnectionManager;
import org.car2x.veins.base.modules.BaseWorldUtility;
import org.car2x.veins.modules.mobility.traci.TraCIScenarioManager*;
import org.car2x.veins.modules.obstacle.ObstacleControl;
import org.car2x.veins.modules.world.annotations.AnnotationManager;
//import org.car2x.veins.nodes.Car;
//import org.car2x.veins.nodes.RSU;

network Scenario
{
    parameters:
        double playgroundSizeX @unit(m); // x size of the area the nodes are in (in meters)
        double playgroundSizeY @unit(m); // y size of the area the nodes are in (in meters)
        double playgroundSizeZ @unit(m); // z size of the area the nodes are in (in meters)

        //@display("i=block/network2;bgb=990.02496,84.575;bgi=background/teste");
        //@display("i=block/network2;bgb=5000.02496,5000.575");
        @display("bgb=$playgroundSizeX,$playgroundSizeY");
        
    submodules:
        //obstacles: ObstacleControl {
        //    @display("p=239,128");
        //}
        annotations: AnnotationManager {
            @display("p=274,251");
        }
        connectionManager: ConnectionManager {
            parameters:
                @display("p=162,251;i=abstract/multicast");
        }
        world: BaseWorldUtility {
            parameters:
                playgroundSizeX = playgroundSizeX;
                playgroundSizeY = playgroundSizeY;
                playgroundSizeZ = playgroundSizeZ;
                @display("p=47,251;i=misc/globe");
        }
        manager: TraCIScenarioManagerLaunchd {
            parameters:
                @display("p=451,251");
        }

        //rsu[1]: RSU {
        rsu[1]: veins_manhattan.src.rsu.MyRSU {
            parameters:
                @display("p=357,251;i=veins/sign/yellowdiamond");
        }
    connections allowunconnected:


}
