package main

import (
	"fmt"
	"github.com/peterbourgon/phonetics"
)

func rhyme(s string) (string, error) {
	m := phonetics.EncodeMetaphone(s)
	if len(m) <= 0 {
		return "", fmt.Errorf("empty input")
	}
	return string(m[len(m)-1]), nil
}

func rhymes(s1, s2 string) bool {
	r1, err := rhyme(s1)
	if err != nil {
		return false
	}
	r2, err := rhyme(s2)
	if err != nil {
		return false
	}
	return r1 == r2
}
