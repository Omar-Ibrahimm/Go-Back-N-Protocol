package proj.simulations;

import proj.Coordinator;
import proj.Node;


@license(LGPL);
//
// TODO documentation
//
network Network
{
    parameters:
        int WS = default(4);
        double TO = default(10);
        double PT = default(0.5);
        double TD = default(1);
        double ED = default(4);
        double DD = default(0.1);
        int LP = default(10);
    submodules:
        node[2]: Node {
            @display("p=132,132");
        }
        coordinator: Coordinator {
            @display("p=221,209");
        }
    connections:
        coordinator.out[0] --> node[0].in[1];
        coordinator.out[1] --> node[1].in[1];
        node[0].out --> node[1].in[0];
        node[1].out --> node[0].in[0];
}

