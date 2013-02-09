package main

import (
	"bufio"
	"encoding/json"
	"fmt"
	"github.com/mrjones/oauth"
	"io/ioutil"
	"log"
	"strings"
)

type Stream struct {
	ConsumerKey    string
	ConsumerSecret string
	AccessToken    string
	AccessSecret   string
}

func NewStream(credentialsFile string) (*Stream, error) {
	buf, err := ioutil.ReadFile(credentialsFile)
	if err != nil {
		return nil, err
	}

	toks := strings.Split(strings.TrimSpace(string(buf)), "\n")
	if len(toks) < 4 {
		return nil, fmt.Errorf("invalid credentials file format")
	}

	return &Stream{
		ConsumerKey:    toks[0],
		ConsumerSecret: toks[1],
		AccessToken:    toks[2],
		AccessSecret:   toks[3],
	}, nil
}

type Tweet struct {
	ID   uint64 `json:"id"`
	User struct {
		ID         uint64 `json:"id"`
		ScreenName string `json:"screen_name"`
	} `json:"user"`
	Text string `json:"text"`
}

func (s *Stream) Consume() (chan Tweet, error) {
	c := oauth.NewConsumer(
		s.ConsumerKey,
		s.ConsumerSecret,
		oauth.ServiceProvider{
			RequestTokenUrl:   "http://api.twitter.com/oauth/request_token",
			AuthorizeTokenUrl: "https://api.twitter.com/oauth/authorize",
			AccessTokenUrl:    "https://api.twitter.com/oauth/access_token",
		},
	)

	response, err := c.Get(
		"https://stream.twitter.com/1.1/statuses/sample.json",
		map[string]string{},
		&oauth.AccessToken{
			Token:  s.AccessToken,
			Secret: s.AccessSecret,
		},
	)
	if err != nil {
		return nil, err
	}

	tweets := make(chan Tweet)
	go func() {
		defer close(tweets)
		defer response.Body.Close()
		r := bufio.NewReader(response.Body)
		for {
			buf, err := r.ReadBytes('\n')
			if err != nil {
				log.Print(err)
				break
			}

			var tweet Tweet
			err = json.Unmarshal(buf, &tweet)
			if err != nil {
				continue
			}
			if tweet.ID == 0 {
				continue
			}
			tweets <- tweet
		}
	}()
	return tweets, nil
}
