	package main

import (
	"bytes"
	"encoding/binary"
	"io"
	"log"
	"net"
//	"unsafe"

	"golang.org/x/text/encoding/unicode"
	"golang.org/x/text/transform"
)

const (
	MT_INIT = iota
	MT_EXIT
	MT_GETDATA
	MT_DATA
	MT_NODATA
	MT_CONFIRM
	MT_ERROR
)

const (
	MR_BROKER = 10
	MR_ALL    = 50
	MR_USER   = 100
)

type MsgHeader struct {
	Type int32
	Size int32
	To   int32
	From int32
}

func (h MsgHeader) Send(conn net.Conn) {
	binary.Write(conn, binary.LittleEndian, h)
}

/*
func (h MsgHeader) Send(conn net.Conn) {
	buff := new(bytes.Buffer)
	binary.Write(buff, binary.LittleEndian, h)
	conn.Write(buff.Bytes())
}
*/

func (h *MsgHeader) Receive(conn net.Conn) {
	err := binary.Read(conn, binary.LittleEndian, h)
	if err != nil {
		h.Size = 0
		h.Type = MT_ERROR
	}
}

/*
func (h *MsgHeader) Receive(conn net.Conn) {
	buff := make([]byte, unsafe.Sizeof(*h))
	_, err := conn.Read(buff)
	if err == nil {
		binary.Read(bytes.NewBuffer(buff), binary.LittleEndian, h)
	} else {
		h.Size = 0
		h.Type = MT_ERROR
	}
}
*/

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

type Message struct {
	Header MsgHeader
	Data   string
}

func makeMessage(To int32, From int32, Type int32, Data string) *Message {
	return &Message{MsgHeader{Type, int32(len([]rune(Data)) * 2), To, From}, Data}
}

func (m Message) Send(conn net.Conn) {
	m.Header.Send(conn)
	if m.Header.Size > 0 {
		conn.Write(toUnicode(m.Data))
	}
}

func (m *Message) Receive(conn net.Conn) int32 {
	m.Header.Receive(conn)
	if m.Header.Size > 0 {
		buff := make([]byte, m.Header.Size)
		conn.Read(buff)
		m.Data = fromUnicode(buff)
	}
	return m.Header.Type
}

var clientID int32 = 0

func MessageSend(conn net.Conn, To int32, From int32, Type int32, Data string) *Message {
	m := makeMessage(To, From, Type, Data)
	m.Send(conn)
	return m
}

func MessageCall(To int32, Type int32, Data string) *Message {
	if conn, err := net.Dial("tcp", "localhost:12345"); err != nil {
		log.Println("Server connection error")
		return makeMessage(clientID, To, MT_NODATA, "")
	} else {
		defer conn.Close()
		m := MessageSend(conn, To, clientID, Type, Data)
		m.Receive(conn)
		if m.Header.Type == MT_INIT {
			clientID = m.Header.To
		}
		return m
	}
}
