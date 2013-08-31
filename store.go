package main

import (
	"fmt"
)

type store map[string]Tweet

func classify(t Tweet) (string, error) {
	m, err := meter(t.DestupifiedText)
	if err != nil {
		return "", err
	}
	if m > 10 {
		return "", fmt.Errorf("too many syllables (%d)", m)
	}
	r, err := rhyme(t.DestupifiedText)
	if err != nil {
		return "", err
	}
	return fmt.Sprintf("%d-%s", m, r), nil
}

func (s store) put(key string, t Tweet) (Tweet, bool) {
	if other, ok := s[key]; ok {
		return other, true
	}
	s[key] = t
	return Tweet{}, false
}
