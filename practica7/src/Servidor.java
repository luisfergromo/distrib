
import java.io.StringReader;
import java.net.DatagramPacket;
import java.net.DatagramSocket;

import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;

import org.w3c.dom.Document;
import org.w3c.dom.Element;
import org.xml.sax.InputSource;

public class Servidor
{
	byte[] buf;
	DatagramSocket socket;
	DatagramPacket packet;

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
		packet = new DatagramPacket(buf, buf.length);

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
				if(Integer.parseInt(element.getElementsByTagName("id").item(0).getTextContent()) == -1)
				{
					if(element.getElementsByTagName("text").item(0).getTextContent().contentEquals("LOGIN_REQUEST"))
					{

					}
				}
				//MSG
				else
				{
					socket.send(packet);
				}
			}
			catch(Exception ex)	{}

			}
		}

	}
