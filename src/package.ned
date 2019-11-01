package speedymurmurs;

@license(LGPL);

simple BasicNode
{
    parameters:
        @display("i=misc/node");
    gates:
        inout g[];
        input in[];
        output out[];
}

network CompleteNetwork7
{
    @display("bgb=1192,503");
    types:
        channel Channel extends ned.DelayChannel
        {
            delay = 10ms;
        }

    submodules:
        node_0: BasicNode {
            @display("p=94,38");
        }
        node_1: BasicNode {

            @display("p=32,100");
        }
        node_2: BasicNode {
            @display("p=157,214");
        }
        node_3: BasicNode {
            @display("p=32,289");
        }
        node_4: BasicNode {
            @display("p=304,214");
        }
        node_5: BasicNode {
            @display("p=436,160");
        }
        node_6: BasicNode {
            @display("p=281,50");
        }
        node_7: BasicNode {
            @display("p=713,116");
        }
        node_8: BasicNode {
            @display("p=537,50");
        }
        node_9: BasicNode {
            @display("p=452,418");
        }
        node_10: BasicNode {
            @display("p=157,355");
        }
        node_11: BasicNode {
            @display("p=304,337");
        }
        node_12: BasicNode {
            @display("p=719,241");
        }
        node_13: BasicNode {
            @display("p=826,202");
        }
        node_14: BasicNode {
            @display("p=725,449");
        }
        node_15: BasicNode {
            @display("p=593,410");
        }
        node_16: BasicNode {
            @display("p=1006,145");
        }
        node_17: BasicNode {
            @display("p=546,175");
        }
        node_18: BasicNode {
            @display("p=973,298");
        }
        node_19: BasicNode {
            @display("p=522,298");
        }
        node_20: BasicNode {
            @display("p=883,362");
        }
        node_21: BasicNode {
            @display("p=934,467");
        }
        node_22: BasicNode {
            @display("p=1059,418");
        }
    connections:
        //Node0
        node_0.out++ --> Channel--> node_1.in++;
        node_0.in++ <-- Channel <-- node_1.out++;

        node_0.out++ --> Channel --> node_6.in++;
        node_0.in++ <-- Channel <-- node_6.out++;


        // Node1
        node_1.out++ --> Channel --> node_2.in++;
        node_1.in++ <-- Channel <-- node_2.out++;

        node_1.out++ --> Channel --> node_3.in++;
        node_1.in++ <-- Channel <-- node_3.out++;

        //Node2
        node_2.out++ --> Channel --> node_3.in++;
        node_2.in++ <-- Channel <-- node_3.out++;

        // Node4
        node_4.out++ --> Channel --> node_5.in++;
        node_4.in++ <-- Channel <-- node_5.out++;

        node_4.out++ --> Channel --> node_6.in++;
        node_4.in++ <-- Channel <-- node_6.out++;

        // Node5
        node_5.out++ --> Channel --> node_6.in++;
        node_5.in++ <-- Channel <-- node_6.out++;

        node_5.out++ --> Channel --> node_17.in++;
        node_17.out++ --> Channel --> node_5.in++;
        node_2.out++ --> Channel --> node_4.in++;
        node_4.out++ --> Channel --> node_2.in++;
        node_2.out++ --> Channel --> node_6.in++;
        node_6.out++ --> Channel --> node_2.in++;
        node_2.out++ --> Channel --> node_11.in++;
        node_11.out++ --> Channel --> node_2.in++;
        node_10.out++ --> Channel --> node_2.in++;
        node_2.out++ --> Channel --> node_10.in++;
        node_10.out++ --> Channel --> node_3.in++;
        node_3.out++ --> Channel --> node_10.in++;
        node_11.out++ --> Channel --> node_10.in++;
        node_10.out++ --> Channel --> node_11.in++;
        node_11.out++ --> Channel --> node_4.in++;
        node_4.out++ --> Channel --> node_11.in++;
        node_2.out++ --> Channel --> node_0.in++;
        node_0.out++ --> Channel --> node_2.in++;
        node_5.out++ --> Channel --> node_8.in++;
        node_8.out++ --> Channel --> node_5.in++;
        node_8.out++ --> Channel --> node_17.in++;
        node_17.out++ --> Channel --> node_8.in++;
        node_5.out++ --> Channel --> node_19.in++;
        node_19.out++ --> Channel --> node_5.in++;
        node_19.out++ --> Channel --> node_17.in++;
        node_17.out++ --> Channel --> node_19.in++;
        node_19.out++ --> Channel --> node_4.in++;
        node_4.out++ --> Channel --> node_19.in++;
        node_19.out++ --> Channel --> node_11.in++;
        node_11.out++ --> Channel --> node_19.in++;
        node_19.out++ --> Channel --> node_9.in++;
        node_9.out++ --> Channel --> node_19.in++;
        node_19.out++ --> Channel --> node_15.in++;
        node_15.out++ --> Channel --> node_19.in++;
        node_9.out++ --> Channel --> node_11.in++;
        node_11.out++ --> Channel --> node_9.in++;
        node_9.out++ --> Channel --> node_15.in++;
        node_15.out++ --> Channel --> node_9.in++;
        node_17.out++ --> Channel --> node_12.in++;
        node_12.out++ --> Channel --> node_17.in++;
        node_17.out++ --> Channel --> node_7.in++;
        node_7.out++ --> Channel --> node_17.in++;
        node_7.out++ --> Channel --> node_12.in++;
        node_12.out++ --> Channel --> node_7.in++;
        node_19.out++ --> Channel --> node_12.in++;
        node_12.out++ --> Channel --> node_19.in++;
        node_19.out++ --> Channel --> node_20.in++;
        node_20.out++ --> Channel --> node_19.in++;
        node_12.out++ --> Channel --> node_13.in++;
        node_13.out++ --> Channel --> node_12.in++;
        node_13.out++ --> Channel --> node_16.in++;
        node_16.out++ --> Channel --> node_13.in++;
        node_13.out++ --> Channel --> node_18.in++;
        node_18.out++ --> Channel --> node_13.in++;
        node_13.out++ --> Channel --> node_20.in++;
        node_20.out++ --> Channel --> node_13.in++;
        node_20.out++ --> Channel --> node_18.in++;
        node_18.out++ --> Channel --> node_20.in++;
        node_20.out++ --> Channel --> node_14.in++;
        node_14.out++ --> Channel --> node_20.in++;
        node_14.out++ --> Channel --> node_15.in++;
        node_15.out++ --> Channel --> node_14.in++;
        node_14.out++ --> Channel --> node_19.in++;
        node_19.out++ --> Channel --> node_14.in++;
        node_15.out++ --> Channel --> node_20.in++;
        node_20.out++ --> Channel --> node_15.in++;
        node_12.out++ --> Channel --> node_20.in++;
        node_20.out++ --> Channel --> node_12.in++;
        node_20.out++ --> Channel --> node_21.in++;
        node_21.out++ --> Channel --> node_20.in++;
        node_20.out++ --> Channel --> node_22.in++;
        node_22.out++ --> Channel --> node_20.in++;
        node_18.out++ --> Channel --> node_22.in++;
        node_22.out++ --> Channel --> node_18.in++;
        node_21.out++ --> Channel --> node_22.in++;
        node_22.out++ --> Channel --> node_21.in++;
        node_21.out++ --> Channel --> node_14.in++;
        node_14.out++ --> Channel --> node_21.in++;
}



