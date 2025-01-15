package main

import (
	"fmt"
	"math/rand"
	"time"

	"github.com/spaolacci/murmur3"
)

type BloomFilter struct {
	filter []bool
	size   int
	seed   uint32
}

func murmerHash(seed uint32, size int, key string) uint32 {
	data := []byte(key)
	hasher := murmur3.New32WithSeed(seed) // Use the seeded hasher
	hasher.Write(data)
	hash := hasher.Sum32()
	return hash % uint32(size)
}

func NewBloomFilter(size int) *BloomFilter {
	// Seed with the current Unix time
	rand.Seed(time.Now().UnixNano())
	seed := uint32(rand.Int31())
	return &BloomFilter{
		filter: make([]bool, size),
		size:   size,
		seed:   seed,
	}
}

func (bloom *BloomFilter) Add(key string) {
	hash := murmerHash(bloom.seed, bloom.size, key)
	fmt.Println("write at %d", hash)
	bloom.filter[hash] = true
}

func (bloom *BloomFilter) Exist(key string) bool {
	hash := murmerHash(bloom.seed, bloom.size, key)
	return bloom.filter[hash]
}

func main() {
	bloomFilter := NewBloomFilter(16)
	keys := []string{"a", "b", "c", "d", "e", "f", "g", "h", "i", "j"}
	for _, key := range keys {
		bloomFilter.Add(key)
	}

	for _, key := range keys {
		exists := bloomFilter.Exist(key)
		if exists {
			println(key, "exists in the Bloom Filter.")
		} else {
			println(key, "does not exist in the Bloom Filter.")
		}
	}

	test := bloomFilter.Exist("x")
	if test {
		println("yes")
	}
}
