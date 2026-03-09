import socket
import time

SERVER_HOST = '127.0.0.1'
SERVER_PORT = 8080

def run_test(category, test_name, raw_bytes, expected_status, fragment=False):
    print(f"[{category}] {test_name}")
    try:
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.settimeout(2.0)
        s.connect((SERVER_HOST, SERVER_PORT))

        if fragment:
            # Simulate a slow client or fragmented packet
            s.sendall(raw_bytes[:len(raw_bytes)//2])
            time.sleep(0.5)
            s.sendall(raw_bytes[len(raw_bytes)//2:])
        else:
            s.sendall(raw_bytes)

        response = s.recv(4096).decode('utf-8', errors='ignore')
        s.close()

        first_line = response.split('\r\n')[0] if response else "EMPTY RESPONSE"
        
        if expected_status in first_line:
            print(f"  ✅ PASS ({first_line})")
        else:
            print(f"  ❌ FAIL (Expected {expected_status}, got: {first_line})")
            
    except Exception as e:
        print(f"  ❌ ERROR: {e}")
    print("-" * 50)

if __name__ == "__main__":
    print(f"Starting WebServ Test Suite on {SERVER_HOST}:{SERVER_PORT}\n")

    # --- CATEGORY: REQUEST LINE ---
    run_test("REQ_LINE", "Valid Request", b"GET / HTTP/1.1\r\nHost: localhost\r\n\r\n", "200")
    run_test("REQ_LINE", "Skip Leading CRLF", b"\r\n\r\nGET / HTTP/1.1\r\nHost: localhost\r\n\r\n", "200")
    run_test("REQ_LINE", "Missing Version", b"GET /\r\nHost: localhost\r\n\r\n", "400")

    # !!!! this tests after fixing partial reading
    #run_test("REQ_LINE", "Fragmented Line", b"GET / HTTP/1.1\r\nHost: localhost\r\n\r\n", "200", fragment=True) 

    # --- CATEGORY: HEADERS ---
    run_test("HEADERS", "Space Before Colon", b"GET / HTTP/1.1\r\nHost : localhost\r\n\r\n", "400")
    # --- CATEGORY: CASE INSENSITIVITY  ---    

    run_test("HEADERS", "Case Insensitivity", b"GET / HTTP/1.1\r\nhOsT: localhost\r\n\r\n", "200")
    run_test("HEADERS", "Case Insensitivity (CAPS)", 
             b"POST / HTTP/1.1\r\nHOST: localhost\r\nCONTENT-LENGTH: 5\r\n\r\nHello", "200")
    
    # --- CATEGORY: DUPLICATE HEADERS ---    
    # Test 1: Duplicate Host (MUST return 400)
    # RFC 9112 Section 3.2.2: A server MUST respond with 400 if multiple Host headers are present.
    run_test("HEADERS", "Duplicate Host (Illegal)", 
             b"GET / HTTP/1.1\r\nHost: a.com\r\nHost: b.com\r\n\r\n", "400")

    # Test 2: Duplicate Content-Length (MUST return 400)
    # Security risk: used for Request Smuggling.
    run_test("HEADERS", "Duplicate Content-Length (Illegal)", 
             b"POST / HTTP/1.1\r\nHost: localhost\r\nContent-Length: 5\r\nContent-Length: 10\r\n\r\nHello", "400")

    # Test 3: Duplicate Accept (Legal - Should return 200)
    # The server should internally store this as: Accept: text/html, text/plain
    run_test("HEADERS", "Duplicate Accept (Legal Append)", 
             b"GET / HTTP/1.1\r\nHost: localhost\r\nAccept: text/html\r\nAccept: text/plain\r\n\r\n", "200")
    
    # --- CATEGORY: SECURITY ---
    run_test("SECURITY", "Smuggling (CL + TE)", 
             b"POST / HTTP/1.1\r\nHost: localhost\r\nContent-Length: 5\r\nTransfer-Encoding: chunked\r\n\r\n0\r\n\r\n", 
             "400")
    """
    # !!!! this tests after fixing partial reading
    # --- CATEGORY: BODY PARSING ---

    # Test 1: Tiny Body (Might pass if the packet is small enough)
    run_test("BODY", "Small POST", 
             b"POST / HTTP/1.1\r\nHost: localhost\r\nContent-Length: 5\r\n\r\nHello", 
             "200")

    # Test 2: Large Body (Likely to fail fragmentation)
    large_body = b"A" * 5000
    run_test("BODY", "Large POST", 
             b"POST / HTTP/1.1\r\nHost: localhost\r\nContent-Length: 5000\r\n\r\n" + large_body, 
             "200", fragment=True)

    # Test 3: Chunked Body (Will fail without state-machine logic)
    # This sends "5 bytes (Hello), then 6 bytes ( World), then end (0)"
    chunked_data = b"5\r\nHello\r\n6\r\n World\r\n0\r\n\r\n"
    run_test("BODY", "Chunked Encoding", 
             b"POST / HTTP/1.1\r\nHost: localhost\r\nTransfer-Encoding: chunked\r\n\r\n" + chunked_data, 
             "200")

    # Test 4: Body size mismatch (Security/Logic test)
    # Content-Length says 10, but we only send 5. Server should not return 200.
    run_test("BODY", "Short Body Mismatch", 
             b"POST / HTTP/1.1\r\nHost: localhost\r\nContent-Length: 10\r\n\r\n12345", 
             "400") # Or it should hang until timeout
             """