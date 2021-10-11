[top]
components : Due@due  relaySYSTEM BS@stats
out : out_port
link : out@due in@relaySYSTEM
link : trow@relaySYSTEM fail@BS
link : hop@relaySYSTEM success@BS
link : out@BS out_port


[relaySYSTEM]
components : Relay1 Relay2 Relay3 Relay4 controller
out : hop trow
in : in   
Link :  relay1Out@controller packetPort@Relay1
Link :  relay2Out@controller packetPort@Relay2
Link :  relay3Out@controller packetPort@Relay3
Link :  relay4Out@controller packetPort@Relay4
Link :  relay4Out@controller packetPort@Relay4
Link :  ProtocolOut@controller ProtocolIn@Relay1
Link :  ProtocolOut@controller ProtocolIn@Relay2
Link :  ProtocolOut@controller ProtocolIn@Relay3
Link :  ProtocolOut@controller ProtocolIn@Relay4
Link :  ProtocolOut@Relay1 ProtocolIn@controller
Link :  ProtocolOut@Relay2 ProtocolIn@controller
Link :  ProtocolOut@Relay3 ProtocolIn@controller
Link :  ProtocolOut@Relay4 ProtocolIn@controller
Link :  retransmit@Relay1 relayIn@controller
Link :  retransmit@Relay2 relayIn@controller  
Link :  retransmit@Relay3 relayIn@controller  
Link :  retransmit@Relay4 relayIn@controller 
Link : in in@controller
Link : hop@Relay1 hop
Link : hop@Relay2 hop
Link : hop@Relay3 hop
Link : hop@Relay4 hop
Link : trow@controller trow

[controller]
components : fifo@Queue Selector@scheduler
out : relay1Out relay2Out relay3Out relay4Out trow  ProtocolOut
in :  in relayIn ProtocolIn
Link : out@fifo queuePort@Selector
Link : ack@Selector done@fifo
Link : ProtocolIn ProtocolIn@Selector
Link : ProtocolOut@Selector ProtocolOut
Link : relay1Out@Selector relay1Out
Link : relay2Out@Selector relay2Out
Link : relay3Out@Selector relay3Out
Link : relay4Out@Selector relay4Out
Link : relayIn relayIn@Selector  
Link : trow@Selector trow
Link : in in@fifo



[Relay1]
components : Transmitter1@transmitter CycleDuty1@cycleScheduler
out : hop ProtocolOut retransmit
in :  packetPort  ProtocolIn
Link : packetPort packetPort@Transmitter1
Link : hop@Transmitter1 hop
Link : ProtocolOut@CycleDuty1 ProtocolOut
Link : ProtocolOut@Transmitter1 ProtocolOut
Link : ProtocolIn ProtocolIn@Transmitter1 
Link : ProtocolIn ProtocolIn@CycleDuty1
Link : retransmit@Transmitter1 retransmit 

[Relay2]
components : Transmitter2@transmitter CycleDuty2@cycleScheduler
out : hop ProtocolOut retransmit
in :  packetPort  ProtocolIn
Link : packetPort packetPort@Transmitter2
Link : hop@Transmitter2 hop
Link : ProtocolOut@CycleDuty2 ProtocolOut
Link : ProtocolOut@Transmitter2 ProtocolOut
Link : ProtocolIn ProtocolIn@Transmitter2 
Link : ProtocolIn ProtocolIn@CycleDuty2
Link : retransmit@Transmitter2 retransmit 

[Relay3]
components : Transmitter3@transmitter CycleDuty3@cycleScheduler
out : hop ProtocolOut retransmit
in :  packetPort  ProtocolIn
Link : packetPort packetPort@Transmitter3
Link : hop@Transmitter3 hop
Link : ProtocolOut@CycleDuty3 ProtocolOut
Link : ProtocolOut@Transmitter3 ProtocolOut
Link : ProtocolIn ProtocolIn@Transmitter3 
Link : ProtocolIn ProtocolIn@CycleDuty3
Link : retransmit@Transmitter3 retransmit 

[Relay4]
components : Transmitter4@transmitter CycleDuty4@cycleScheduler
out : hop ProtocolOut retransmit
in :  packetPort  ProtocolIn
Link : packetPort packetPort@Transmitter4
Link : hop@Transmitter4 hop
Link : ProtocolOut@CycleDuty4 ProtocolOut
Link : ProtocolOut@Transmitter4 ProtocolOut
Link : ProtocolIn ProtocolIn@Transmitter4 
Link : ProtocolIn ProtocolIn@CycleDuty4
Link : retransmit@Transmitter4 retransmit 



[Due]
lambda : 0.5

[Transmitter1]
r2 : 500
mu : 1
r : 10
size : 4
l: 2
noise : 50
alpha : 2
Px : 13
device_density : 20

[Transmitter2]
r2 : 500
mu : 1
r : 10
size : 4
l: 2
noise : 50
alpha : 2
Px : 13
device_density : 20

[Transmitter3]
r2 : 500
mu : 1
r : 10
size : 4
l: 2
noise : 50
alpha : 2
Px : 13
device_density : 20


[Transmitter4]
r2 : 500
mu : 1
r : 10
size : 4
l: 2
noise : 50
alpha : 2
Px : 13
device_density : 20


[CycleDuty1]
c : 0.4

[CycleDuty2]
c : 0.4

[CycleDuty3]
c : 0.4

[CycleDuty4]
c : 0.4

[Selector]
N : 4

[fifo]
preparation : ''

