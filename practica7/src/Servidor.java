
import java.io.StringReader;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.util.ArrayList;

import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;

import org.w3c.dom.Document;
import org.w3c.dom.Element;
import org.xml.sax.InputSource;

public class Servidor
{
	byte[] buf;
	DatagramSocket socket;
	ArrayList<Session> sessions;

	public static void main(String [] args)
	{
		try
		{
			int port = 9931;
			int buffLen = 512;
			new Servidor(port,buffLen);
		}
		catch(Exception ex)
		{
			System.out.println(ex.getMessage());
		}
	}

	public Servidor(int port,int buffLen) throws Exception
	{
		buf = new byte[buffLen];
		socket = new DatagramSocket(port);
		sessions= new ArrayList<Session>();

		System.out.println("Server Up");
		System.out.println("Listening to port "+port);

		run();
	}

	void run() throws Exception
	{
		while(true)
		{
			System.out.println("listening... ");
			String msg		= "";
			DatagramPacket packet= new DatagramPacket(buf, buf.length);
			socket.receive(packet);

			//Se adapta el mensaje leido para entenderlo
			for(byte b: packet.getData())
			{
				msg+= (char) b;
				if(msg.endsWith("</socket>"))
				break;
			}

			try
			{
				System.out.println(msg);

				//Se deserializa el XML
				DocumentBuilderFactory dbf = DocumentBuilderFactory.newInstance();
				DocumentBuilder db = dbf.newDocumentBuilder();
				InputSource is = new InputSource();
				is.setCharacterStream(new StringReader(msg));
				Document doc = db.parse(is);
				Element element = (Element) doc.getElementsByTagName("socket").item(0);

				//LOGIN
				if(element.getElementsByTagName("id").item(0).getTextContent().contentEquals("-1"))
				{
					if(element.getElementsByTagName("text").item(0).getTextContent().contentEquals("LOGIN_REQUEST"))
						sessions.add(new Session(packet, element));
				}
				//MSG BROADCAST
				else if(element.getElementsByTagName("dest").item(0).getTextContent().contentEquals("0"))
				{
					String src= element.getElementsByTagName("src").item(0).getTextContent();
					for(Session session: sessions)
						if(!src.contentEquals(session.nickname))
						{
							DatagramPacket reply= new DatagramPacket(packet.getData().clone(), packet.getLength(), session.address, session.port);
							socket.send(reply);
						}
				}
				//PRIVATE MSG
				else
				{
					String dest= element.getElementsByTagName("dest").item(0).getTextContent();
					for(Session session: sessions)
						if(dest.contentEquals(session.nickname))
						{
							DatagramPacket reply= new DatagramPacket(packet.getData().clone(), packet.getLength(), session.address, session.port);
							socket.send(reply);
						}
				}
			}
			catch(Exception ex)	{}

			}
		}

		class Session
		{
			public String nickname;
			public InetAddress address;
			public int port;

			public Session(DatagramPacket data, Element xmlMsg)
			{
				this.nickname= xmlMsg.getElementsByTagName("src").item(0).getTextContent();
				this.address= data.getAddress();
				this.port= data.getPort();
			}
		}

	}
