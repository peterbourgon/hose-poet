package main

type Classifier interface {
	Classify(Tweet) (string, error)
}

type Store struct {
	cl Classifier
	m  map[string]Tweet // classification: most recent Tweet
}

func NewStore(cl Classifier) *Store {
	return &Store{
		cl: cl,
		m:  map[string]Tweet{},
	}
}

func (s *Store) Feed(tweet Tweet) ([]Tweet, bool) {
	classification, err := s.cl.Classify(tweet)
	if err != nil {
		//log.Printf("x %30d %20s -- %s", tweet.ID, err, tweet.Text)
		return []Tweet{}, false
	}
	//log.Printf("• %30d %20s -- %s", tweet.ID, classification, tweet.Text)

	if _, ok := s.m[classification]; ok {
		//log.Printf("• %30d %20s -- %s", tweet.ID, "--MATCH--", "")
		match := []Tweet{s.m[classification], tweet}
		delete(s.m, classification)
		return match, true
	}

	//log.Printf("• %30d %20s -- %s", tweet.ID, "(store)", "")
	s.m[classification] = tweet
	return []Tweet{}, false
}
