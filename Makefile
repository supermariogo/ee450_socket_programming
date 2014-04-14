nunki: server bidder seller 
	mv bidder.out bidder1.out
	cp bidder1.out bidder2.out
	mv seller.out seller1.out
	cp seller1.out seller2.out
	@ echo "\n----------------*.txt--------------------\n"
	@ ls *.txt
	@ echo "\n----------------*.out--------------------\n"
	@ ls *.out

server: auctionserver.h auctionserver.c
	gcc -o server.out -g -Wall auctionserver.c -lpthread -lsocket -lnsl -lresolv 

bidder: bidder.c bidder.h
	gcc -o bidder.out -g -Wall bidder.c -lpthread -lsocket -lnsl -lresolv

seller: seller.h seller.c
	gcc -o seller.out -g -Wall seller.c -lpthread -lsocket -lnsl -lresolv

u: server_u bidder_u seller_u 
	mv bidder.out bidder1.out
	cp bidder1.out bidder2.out
	mv seller.out seller1.out
	cp seller1.out seller2.out
	@ echo "\n----------------*.txt--------------------\n"
	@ ls *.txt
	@ echo "\n----------------*.out--------------------\n"
	@ ls *.out

server_u: auctionserver.h auctionserver.c
	gcc -o server.out -g -Wall auctionserver.c -lpthread  

bidder_u: bidder.c bidder.h
	gcc -o bidder.out -g -Wall bidder.c -lpthread 

seller_u: seller.h seller.c
	gcc -o seller.out -g -Wall seller.c -lpthread 


display: 
	@ echo "\n----------------*.txt--------------------\n"
	@ ls *.txt
	@ echo "\n----------------*.out--------------------\n"
	@ ls *.out
	@ echo "\n"

clean:
	rm -f *.o *~

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
	
	
