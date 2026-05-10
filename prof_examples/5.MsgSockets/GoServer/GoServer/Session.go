package main

import "net"

type Session struct {
	id       int32
	name     string
	messages chan *Message
}

func (session *Session) Add(m *Message) {
	session.messages <- m
}

func (session *Session) Send(conn net.Conn) {
	select {
	case m := <-session.messages:
		m.Send(conn)
	default:
		MessageSend(conn, session.id, MR_BROKER, MT_NODATA, "")
	}
}
