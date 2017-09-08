import MySQLdb
import RPi.GPIO as GPIO
from lib_nrf24 import NRF24
import time
import spidev
import datetime
#from MySQLdb import OperationalError # to check if the mysql server has gone away

GPIO.setmode(GPIO.BCM)

pipes = [[0xE8, 0xE8, 0xF0, 0xF0, 0xE1], [0xF0, 0xF0, 0xF0, 0xF0, 0xE1]]

radio = NRF24(GPIO, spidev.SpiDev())
radio.begin(0,17) #valeur de cso

radio.setPayloadSize(32)
radio.setChannel(0x60)
radio.setDataRate(NRF24.BR_1MBPS) #thesloweryougobetteryourrangeis
radio.setPALevel(NRF24.PA_MIN)#conservenergyorimprve range

radio.setAutoAck(True)
radio.enableDynamicPayloads()
radio.enableAckPayload()

radio.openWritingPipe(pipes[0])
radio.openReadingPipe(1, pipes[1])#pipe[1]0xF0, 0xF0, 0xF0, 0xF0, 0xE1
radio.printDetails()

#my_bool reconnect = 0;

message = list("GETDATA") # si on met que cette commande erreur => duplication
while len(message)< 32:
    message.append(0) # c'est pour s'occuper des bits non utilisees

#connection au serveur mysql
conn = MySQLdb.connect(host="localhost",user="root",passwd="bresil",db="donnees_capt")
cursor = conn.cursor()


while True:

        start = time.time()
	radio.write(message)
    	print("Sent the message: {}".format(message))
		
        radio.startListening()

    	while not radio.available(0):
        	time.sleep(1/100)
                if time.time() -start > 2:
                    	print("Timed out.")
                    	break


    	receivedMessage = [] #bite/time bite=8its => 0 265 bytes 32 numbers between 0 265
    	radio.read(receivedMessage, radio.getDynamicPayloadSize())
    	print("Received: {}".format(receivedMessage))

	
    	print("Translating our received message into unicode characters ...")
    	messagedecode = ""

    	for n in receivedMessage:
        	if (n >= 32 and n <= 126):
            		messagedecode += chr(n)
    		print("Our received message decodes to:{}".format(messagedecode))

	if messagedecode :
    	
    		#separation des valeurs
		Temp = messagedecode.split(";")[0]
		Humid = messagedecode.split(";")[1]
		Lum = messagedecode.split(";")[2]
	
		#cursor.execute("INSERT INTO donnees_capt(temperature) VALUES (%s)", (Temp)); #on selectionne la table
		sql = ("INSERT INTO donnees_capt (temperature,humidity,luminosity,date_hour) VALUES (%s,%s,%s,CURRENT_TIMESTAMP())", (Temp,Humid,Lum))
		
		cursor.execute(*sql)
		conn.commit()


		#try :
			#cursor.execute(*sql)
			#conn.commit()
	
		#try :
    			#print "essai d'ecriture"
    			#cursor.execute(*sql)
    			#conn.commit()
   			#print "ecriture finie" 
		
    
		#except :
    			#conn.rollback()
    			#print "FAIL !!!!"

			

		cursor.close()
		conn.close()

		radio.stopListening()

		#time.sleep(240) # 4 minutes de veille/pause entre chaque mesure
	

		break




