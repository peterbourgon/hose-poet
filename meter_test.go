package main

import (
	"testing"
)

func TestMeter(t *testing.T) {
	for word, expected := range map[string]int{
		"foo":           1,
		"define":        2,
		"observe":       2,
		"consuming":     3,
		"deliberate":    4,
		"independently": 5,
	} {
		got, err := meter(word)
		if err != nil {
			t.Errorf("%15s: %s", word, err)
			continue
		}
		if expected != got {
			t.Errorf("%15s: expected %d, got %d", word, expected, got)
			continue
		}
		t.Logf("%15s: %d OK", word, got)
	}
}
