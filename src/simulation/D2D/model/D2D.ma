[top]
components : Due@due  relaySYSTEM
out : out_port
link : out@due in@relaySYSTEM
link : out@relaySYSTEM out_port

[relaySYSTEM]
components : Relay1 Relay2 Relay3 Relay4 controller
out : out stats
in : in   
Link :  relay1Out@controller packetPort@Relay1
Link :  relay2Out@controller packetPort@Relay2
Link :  relay3Out@controller packetPort@Relay3
Link :  relay4Out@controller packetPort@Relay4
Link :  protocol@Relay1 relayIn@controller
Link :  protocol@Relay2 relayIn@controller  
Link :  protocol@Relay3 relayIn@controller  
Link :  protocol@Relay4 relayIn@controller
Link :  updatePort@Relay1 protocolPort@controller
Link :  updatePort@Relay2 protocolPort@controller  
Link :  updatePort@Relay3 protocolPort@controller  
Link :  updatePort@Relay4 protocolPort@controller  
Link : in in@controller
Link : hop@Relay1 out
Link : hop@Relay2 out
Link : hop@Relay3 out
Link : hop@Relay4 out
Link : trow@controller stats

[controller]
components : fifo@Queue Selector@scheduler
out : relay1Out relay2Out relay3Out relay4Out trow
in :  in relayIn protocolPort
Link : out@fifo queuePort@Selector
Link : ack@Selector done@fifo
Link : relay1Out@Selector relay1Out
Link : relay2Out@Selector relay2Out
Link : relay3Out@Selector relay3Out
Link : relay4Out@Selector relay4Out
Link : relayIn relayIn@Selector 
Link : protocolPort protocolPort@Selector 
Link : trow@Selector trow
Link : in in@fifo



[Relay1]
components : Transmitter1@transmitter CycleDuty1@cycleScheduler
out : hop protocol updatePort 
in :  packetPort  
Link : packetPort packetPort@Transmitter1
Link : hop@Transmitter1 hop
Link : updatePort@CycleDuty1 updatePort
Link : protocol@Transmitter1 protocol

[Relay2]
components : Transmitter2@transmitter CycleDuty2@cycleScheduler
out : hop protocol updatePort 
in :  packetPort  
Link : packetPort packetPort@Transmitter2
Link : hop@Transmitter2 hop
Link : updatePort@CycleDuty2 updatePort
Link : protocol@Transmitter2 protocol

[Relay3]
components : Transmitter3@transmitter CycleDuty3@cycleScheduler
out : hop protocol updatePort 
in :  packetPort  
Link : packetPort packetPort@Transmitter3
Link : hop@Transmitter3 hop
Link : updatePort@CycleDuty3 updatePort
Link : protocol@Transmitter3 protocol

[Relay4]
components : Transmitter4@transmitter CycleDuty4@cycleScheduler
out : hop protocol updatePort 
in :   packetPort  
Link : packetPort packetPort@Transmitter4
Link : hop@Transmitter4 hop
Link : updatePort@CycleDuty4 updatePort
Link : protocol@Transmitter4 protocol



[Due]
lambda : 0.5

[Transmitter1]
mu : 1.5
r : 10
size : 4
l: 2
noise : 50
alpha : 1.3
Px : 13
device_density : 500
tau : 10

[Transmitter2]
mu : 1.5
r : 10
size : 4
l: 2
noise : 50
alpha : 1.3
Px : 13
device_density : 500
tau : 10

[Transmitter3]
mu : 1.5
r : 10
size : 4
l: 2
noise : 50
alpha : 1.3
Px : 13
device_density : 500
tau : 10

[Transmitter4]
mu : 1.5
r : 10
size : 4
l: 2
noise : 50
alpha : 1.3
Px : 13
device_density : 500
tau : 10

[CycleDuty1]
c : 0.2

[CycleDuty2]
c : 0.2

[CycleDuty3]
c : 0.2

[CycleDuty4]
c : 0.2

[Selector]
N : 4

[fifo]
preparation : ''
