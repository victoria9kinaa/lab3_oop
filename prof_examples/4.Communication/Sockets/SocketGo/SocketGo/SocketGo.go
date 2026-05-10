package main

import (
	"bytes"
	"encoding/binary"
	"fmt"
	"golang.org/x/text/encoding/unicode"
	"golang.org/x/text/transform"
	"io"
	"log"
	"net"
	"os"
)

func fromUnicode(b []byte) string {
	reader := transform.NewReader(bytes.NewReader(b), unicode.UTF16(unicode.LittleEndian, unicode.IgnoreBOM).NewDecoder())
	r, _ := io.ReadAll(reader)
	return string(r)
}

func toUnicode(s string) []byte {
	buff := new(bytes.Buffer)
	writer := transform.NewWriter(buff, unicode.UTF16(unicode.LittleEndian, unicode.IgnoreBOM).NewEncoder())
	defer writer.Close()
	writer.Write([]byte(s))
	return buff.Bytes()
}

type String string

func (s String) toUnicode() []byte {
	buff := new(bytes.Buffer)
	writer := transform.NewWriter(buff, unicode.UTF16(unicode.LittleEndian, unicode.IgnoreBOM).NewEncoder())
	defer writer.Close()
	writer.Write([]byte(string(s)))
	return buff.Bytes()
}

func receiveString(conn net.Conn) string {
	var size int32
	binary.Read(conn, binary.LittleEndian, &size)
	buff := make([]byte, size)
	conn.Read(buff)
	return fromUnicode(buff)
}

func sendString(conn net.Conn, s string) {
	size := int32(len([]rune(s)) * 2)
	binary.Write(conn, binary.LittleEndian, size)
	conn.Write(toUnicode(s))
}

func main() {
	var connect = "127.0.0.1:12345"
	if len(os.Args) > 1 {
		connect =  os.Args[1]
	}
	if conn, err := net.Dial("tcp", connect); err != nil {
		log.Println("Server connection error")
	} else {
		defer conn.Close()
		for {
			var s string
			fmt.Scanln(&s)
			sendString(conn, s)
			fmt.Println(receiveString(conn))
		}
	}
}
