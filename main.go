package main

import (
	"flag"
	"fmt"
	"io/ioutil"
	"log"
	"strings"
)

func main() {
	var (
		credentials = flag.String("credentials", "CREDENTIALS", "credentials cache file")
	)
	flag.Parse()

	log.SetFlags(0)

	buf, err := ioutil.ReadFile(*credentials)
	if err != nil {
		log.Fatal(err)
	}

	stream, err := newStream(buf)
	if err != nil {
		log.Fatal(err)
	}

	func() {
		for _, s := range []string{
			"i want u to stay",
			"thats wat she said hah",
		} {
			syll, err := syllablez(s)
			if err != nil {
				panic(err)
			}
			rhym, err := rhymez(strings.Split(s, " ")[4])
			if err != nil {
				panic(err)
			}
			log.Printf("%s: %d %v", s, syll, rhym)
		}
	}()

	tweets, err := stream.consume()
	if err != nil {
		log.Fatal(err)
	}

	fwd := map[string]uint64{}   // word -> rhyming tweet ID
	rev := map[uint64][]string{} // tweet ID -> rhyming words
	con := map[uint64]Tweet{}    // tweet ID -> tweet
	for tweet := range tweets {

		syllables, err := syllablez(tweet.DestupifiedText)
		if err != nil {
			continue
		}

		key := fmt.Sprintf("%d%s", syllables, tweet.LastWord)
		log.Printf("%d => %-140s => %s", tweet.ID, tweet.DestupifiedText, key)
		if matchID, ok := fwd[key]; ok {
			log.Println()
			log.Printf("MATCH %d %-16s %s", matchID, con[matchID].User.ScreenName, con[matchID].DestupifiedText)
			log.Printf("MATCH %d %-16s %s", tweet.ID, tweet.User.ScreenName, tweet.DestupifiedText)
			log.Println()
			for _, word := range rev[matchID] {
				delete(fwd, word)
			}
			delete(con, matchID)
			delete(rev, matchID)
			continue
		}

		candidates, err := rhymez(tweet.LastWord)
		if err != nil {
			continue
		}
		for i := range candidates {
			candidates[i] = fmt.Sprintf("%d%s", syllables, candidates[i])
			fwd[candidates[i]] = tweet.ID
		}
		rev[tweet.ID] = candidates
		con[tweet.ID] = tweet

	}
}
