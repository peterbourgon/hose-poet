package main

import (
	"bufio"
	"bytes"
	"encoding/json"
	"fmt"
	"github.com/mrjones/oauth"
	"log"
	"strings"
)

type stream struct {
	ConsumerKey    string
	ConsumerSecret string
	AccessToken    string
	AccessSecret   string
}

func newStream(credentials []byte) (*stream, error) {
	toks := bytes.Split(bytes.TrimSpace(credentials), []byte{'\n'})
	if len(toks) < 4 {
		return nil, fmt.Errorf("invalid credentials file format")
	}

	return &stream{
		ConsumerKey:    string(toks[0]),
		ConsumerSecret: string(toks[1]),
		AccessToken:    string(toks[2]),
		AccessSecret:   string(toks[3]),
	}, nil
}

type Tweet struct {
	ID   uint64 `json:"id"`
	User struct {
		ID         uint64 `json:"id"`
		ScreenName string `json:"screen_name"`
	} `json:"user"`
	Text            string `json:"text"`
	DestupifiedText string
	LastWord        string
}

func (s *stream) consume() (chan Tweet, error) {
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
		map[string]string{"language": "en"},
		&oauth.AccessToken{
			Token:  s.AccessToken,
			Secret: s.AccessSecret,
		},
	)
	if err != nil {
		return nil, err
	}

	tweets := make(chan Tweet)
	newlineRemover := strings.NewReplacer("\r", "", "\n", "")
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
			tweet.Text = newlineRemover.Replace(tweet.Text)
			tweet.DestupifiedText, err = destupify(tweet.Text)
			if err != nil {
				continue
			}
			words := strings.Split(tweet.DestupifiedText, " ")
			if len(words) <= 0 {
				continue
			}
			tweet.LastWord = words[len(words)-1]
			if tweet.LastWord == "" {
				continue
			}
			tweets <- tweet
		}
	}()
	return tweets, nil
}
