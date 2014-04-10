default: auctionserver.o bidder.o seller.o main.o 
	g++ -o auction -g auctionserver.o bidder.o seller.o main.o

auctionserver.o: auctionserver.h auctionserver.cpp
	g++ -g -c -Wall auctionserver.cpp

bidder.o: bidder.cpp bidder.h
	g++ -g -c -Wall bidder.cpp

seller.o: seller.h seller.cpp
	g++ -g -c -Wall seller.cpp

main.o:	main.h main.cpp
	g++ -g -c -Wall main.cpp

clean:
	rm -f *.o *~ auction
