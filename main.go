package main

import (
	"flag"
	"log"
)

var (
	credentials = flag.String("credentials", "CREDENTIALS", "credentials cache file")
)

func init() {
	flag.Parse()
}

func main() {
	stream, err := NewStream(*credentials)
	if err != nil {
		log.Println(err)
		log.Println("To create a credentials file,")
		log.Println(" go get github.com/peterbourgon/hose-poet/make-credentials")
		log.Println("then run make-credentials")
		log.Fatal("")
	}

	tweets, err := stream.Consume()
	if err != nil {
		log.Fatal(err)
	}

	for tweet := range tweets {
		log.Printf("%20s: %s", tweet.User.ScreenName, tweet.Text)
	}
}
