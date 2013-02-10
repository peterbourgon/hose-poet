package main

import (
	"log"
)

type Classifier interface {
	Classify(*Tweet) (string, error)
	Match(c1, c2 string) bool
}

type PoetryBox struct {
	cl Classifier
	m  map[string]*Tweet // encoded meter/rhyme scheme: most recent Tweet
}

func NewPoetryBox(cl Classifier) *PoetryBox {
	return &PoetryBox{
		cl: cl,
		m:  map[string]*Tweet{},
	}
}

type Couplet struct {
	First  *Tweet
	Second *Tweet
}

func (pb *PoetryBox) Feed(tweet *Tweet) (Couplet, bool) {
	classification, err := pb.cl.Classify(tweet)
	if err != nil {
		return Couplet{}, false
	}
	log.Printf("%s", classification)

	for classification0, tweet0 := range pb.m {
		if pb.cl.Match(classification, classification0) {
			if tweet.ID == tweet0.ID {
				continue
			}
			return Couplet{tweet0, tweet}, true
		}
	}
	pb.m[classification] = tweet
	return Couplet{}, false
}
