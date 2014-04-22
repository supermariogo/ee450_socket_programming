nunki: MyList port server bidder seller 
	@ echo "************************************************************"
	@ echo "if ports have already been used, please kill the processes using my ports"
	@ echo "my ports are: 2039 2139 4039 4139 3039 3139 5039 5139"
	@ echo "my last 3 digits are 939"
	@ echo "************************************************************"

port:
	gcc port.c -o port.o -DSERVERHOST=NUNKI -g -c -Wall
server: 
	gcc auctionserver.c port.o MyList.o -o server.o -DSERVERHOST=NUNKI -g -Wall -lpthread -lsocket -lnsl -lresolv 

bidder: 
	gcc bidder.c port.o -o bidder1.o -DBIDDERX=1 -DSERVERHOST=NUNKI -g -Wall -lpthread -lsocket -lnsl -lresolv
	gcc bidder.c port.o -o bidder2.o -DBIDDERX=2 -DSERVERHOST=NUNKI -g -Wall -lpthread -lsocket -lnsl -lresolv
                                     
seller:                              
	gcc seller.c port.o -o seller1.o -DSELLERX=1 -DSERVERHOST=NUNKI -g -Wall -lpthread -lsocket -lnsl -lresolv
	gcc seller.c port.o -o seller2.o -DSELLERX=2 -DSERVERHOST=NUNKI -g -Wall -lpthread -lsocket -lnsl -lresolv

u: MyList port_u server_u bidder_u seller_u 
	@ echo "************************************************************"
	@ echo "if ports have already been used, please kill the processes using my ports"
	@ echo "my ports are: 2039 2139 4039 4139 3039 3139 5039 5139"
	@ echo "my last 3 digits are 939"
	@ echo "************************************************************"


port_u: 
	gcc port.c -o port.o -DSERVERHOST=LOCALHOST -g -c -Wall

server_u: 
	gcc auctionserver.c MyList.o port.o -o server.o -DSERVERHOST=LOCALHOST -g -Wall -lpthread  

bidder_u: 
	gcc bidder.c  port.o -o bidder1.o -DBIDDERX=1 -DSERVERHOST=LOCALHOST -g -Wall -lpthread
	gcc bidder.c  port.o -o bidder2.o -DBIDDERX=2 -DSERVERHOST=LOCALHOST -g -Wall -lpthread

seller_u: 
	gcc seller.c  port.o -o seller1.o -DSELLERX=1 -DSERVERHOST=LOCALHOST -g -Wall -lpthread
	gcc seller.c  port.o -o seller2.o -DSELLERX=2 -DSERVERHOST=LOCALHOST -g -Wall -lpthread

MyList:
	gcc MyList.c -o MyList.o -g -c -Wall

clean:
	rm -f *.o *~ *.out *.so

cat1:
	cat Registration.txt
	@echo "----------------------------------------------------"
	@cat bidderPass1.txt
	@cat bidderPass2.txt
	@cat sellerPass1.txt
	@cat sellerPass2.txt
cat2:
	cat broadcastList.txt
	@echo "------------------------seller1 sell----------------------------"
	cat itemList1.txt
	@echo "------------------------seller2 sell----------------------------"
	cat itemList2.txt
	@echo "------------------------bidder1 bidding----------------------------"
	cat bidding1.txt
	@echo "------------------------bidder2 bidding----------------------------"
	cat bidding2.txt

run:
	gnome-terminal -t "server" -x bash -c "./server.o;exec bash;" &		
	gnome-terminal -t "bidder1" -x bash -c "./bidder1.o;exec bash;" &
	gnome-terminal -t "bidder2" -x bash -c "./bidder2.o;exec bash;" &
	gnome-terminal -t "seller1" -x bash -c "./seller1.o;exec bash;" &
	gnome-terminal -t "seller2" -x bash -c "./seller2.o;exec bash;" &
	
user:
	./bidder1.o &
	./bidder2.o &	
	./seller1.o &
	./seller2.o &
