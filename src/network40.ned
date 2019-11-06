simple BasicNode {
        
    gates:
        input in[];
        output out[];
}


network Network40 {
    
types:
    channel Channel extends ned.DelayChannel
    {
        delay = 10ms;
    }
    
submodules:
        node00000 : BasicNode;
        node00001 : BasicNode;
        node00002 : BasicNode;
        node00003 : BasicNode;
        node00004 : BasicNode;
        node00005 : BasicNode;
        node00006 : BasicNode;
        node00007 : BasicNode;
        node00008 : BasicNode;
        node00009 : BasicNode;
        node00010 : BasicNode;
        node00011 : BasicNode;
        node00012 : BasicNode;
        node00013 : BasicNode;
        node00014 : BasicNode;
        node00015 : BasicNode;
        node00016 : BasicNode;
        node00017 : BasicNode;
        node00018 : BasicNode;
        node00019 : BasicNode;
        node00020 : BasicNode;
        node00021 : BasicNode;
        node00022 : BasicNode;
        node00023 : BasicNode;
        node00024 : BasicNode;
        node00025 : BasicNode;
        node00026 : BasicNode;
        node00027 : BasicNode;
        node00028 : BasicNode;
        node00029 : BasicNode;
        node00030 : BasicNode;
        node00031 : BasicNode;
        node00032 : BasicNode;
        node00033 : BasicNode;
        node00034 : BasicNode;
        node00035 : BasicNode;
        node00036 : BasicNode;
        node00037 : BasicNode;
        node00038 : BasicNode;
        node00039 : BasicNode;

connections:
node00000.out++ --> Channel --> node00001.in++;
node00000.in++ <-- Channel <-- node00001.out++;
node00000.out++ --> Channel --> node00002.in++;
node00000.in++ <-- Channel <-- node00002.out++;
node00000.out++ --> Channel --> node00003.in++;
node00000.in++ <-- Channel <-- node00003.out++;
node00000.out++ --> Channel --> node00004.in++;
node00000.in++ <-- Channel <-- node00004.out++;
node00000.out++ --> Channel --> node00010.in++;
node00000.in++ <-- Channel <-- node00010.out++;
node00000.out++ --> Channel --> node00013.in++;
node00000.in++ <-- Channel <-- node00013.out++;
node00000.out++ --> Channel --> node00017.in++;
node00000.in++ <-- Channel <-- node00017.out++;
node00000.out++ --> Channel --> node00022.in++;
node00000.in++ <-- Channel <-- node00022.out++;
node00000.out++ --> Channel --> node00029.in++;
node00000.in++ <-- Channel <-- node00029.out++;
node00000.out++ --> Channel --> node00030.in++;
node00000.in++ <-- Channel <-- node00030.out++;
node00000.out++ --> Channel --> node00031.in++;
node00000.in++ <-- Channel <-- node00031.out++;
node00000.out++ --> Channel --> node00038.in++;
node00000.in++ <-- Channel <-- node00038.out++;
node00001.out++ --> Channel --> node00005.in++;
node00001.in++ <-- Channel <-- node00005.out++;
node00001.out++ --> Channel --> node00024.in++;
node00001.in++ <-- Channel <-- node00024.out++;
node00001.out++ --> Channel --> node00025.in++;
node00001.in++ <-- Channel <-- node00025.out++;
node00001.out++ --> Channel --> node00026.in++;
node00001.in++ <-- Channel <-- node00026.out++;
node00002.out++ --> Channel --> node00011.in++;
node00002.in++ <-- Channel <-- node00011.out++;
node00002.out++ --> Channel --> node00023.in++;
node00002.in++ <-- Channel <-- node00023.out++;
node00002.out++ --> Channel --> node00035.in++;
node00002.in++ <-- Channel <-- node00035.out++;
node00004.out++ --> Channel --> node00006.in++;
node00004.in++ <-- Channel <-- node00006.out++;
node00004.out++ --> Channel --> node00007.in++;
node00004.in++ <-- Channel <-- node00007.out++;
node00004.out++ --> Channel --> node00008.in++;
node00004.in++ <-- Channel <-- node00008.out++;
node00004.out++ --> Channel --> node00009.in++;
node00004.in++ <-- Channel <-- node00009.out++;
node00004.out++ --> Channel --> node00014.in++;
node00004.in++ <-- Channel <-- node00014.out++;
node00004.out++ --> Channel --> node00020.in++;
node00004.in++ <-- Channel <-- node00020.out++;
node00010.out++ --> Channel --> node00016.in++;
node00010.in++ <-- Channel <-- node00016.out++;
node00022.out++ --> Channel --> node00032.in++;
node00022.in++ <-- Channel <-- node00032.out++;
node00007.out++ --> Channel --> node00015.in++;
node00007.in++ <-- Channel <-- node00015.out++;
node00007.out++ --> Channel --> node00018.in++;
node00007.in++ <-- Channel <-- node00018.out++;
node00007.out++ --> Channel --> node00028.in++;
node00007.in++ <-- Channel <-- node00028.out++;
node00007.out++ --> Channel --> node00034.in++;
node00007.in++ <-- Channel <-- node00034.out++;
node00007.out++ --> Channel --> node00037.in++;
node00007.in++ <-- Channel <-- node00037.out++;
node00008.out++ --> Channel --> node00012.in++;
node00008.in++ <-- Channel <-- node00012.out++;
node00009.out++ --> Channel --> node00019.in++;
node00009.in++ <-- Channel <-- node00019.out++;
node00014.out++ --> Channel --> node00033.in++;
node00014.in++ <-- Channel <-- node00033.out++;
node00034.out++ --> Channel --> node00036.in++;
node00034.in++ <-- Channel <-- node00036.out++;
node00019.out++ --> Channel --> node00021.in++;
node00019.in++ <-- Channel <-- node00021.out++;
node00021.out++ --> Channel --> node00027.in++;
node00021.in++ <-- Channel <-- node00027.out++;
node00032.out++ --> Channel --> node00039.in++;
node00032.in++ <-- Channel <-- node00039.out++;
}
