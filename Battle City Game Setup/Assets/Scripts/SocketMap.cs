using UnityEngine;
using UnityEngine.Tilemaps;
using System;
using System.Net.Sockets;
using System.Text;
using System.IO;
using System.Threading;
using System.Threading.Tasks;
using System.Collections;

public class SocketMap : MonoBehaviour
{
    public Transform eagle;
    private MapLoad mapLoad;
    private bool isRunning = true;
    async void Start()
    {
        await ConnectToServer();
    }

    void Awake() {
        mapLoad = FindAnyObjectByType<MapLoad>();
    }
    void OnApplicationQuit() {
        isRunning = false;
    }

    async Task ConnectToServer()
    {
        try
        {
            // Nếu dùng WSL2, dùng "127.0.0.1". Nếu dùng VM, dùng IP của máy ảo.
            string ipAddress = "127.0.0.1";
            int port = 8080;

            TcpClient client = new TcpClient(ipAddress, port);
            NetworkStream stream = client.GetStream();

            while (isRunning) {
                await Task.Delay(1000);
                string[] m = System.IO.File.ReadAllLines(@"Assets/Maps/map" + mapLoad.GetCurrentLevel() + ".txt");
                StringBuilder sb = new StringBuilder();
                string agentData = GetAgentData();
                string taskData = GetTaskData();

                sb.AppendLine("type octile");
                sb.AppendLine("height 26");
                sb.AppendLine("width 26");
                sb.AppendLine("map");

                foreach (string line in m) {
                    sb.AppendLine(line);
                }

                string dataToSend = sb.ToString() + "---" + agentData + "\n---" + taskData;

                byte[] data = Encoding.ASCII.GetBytes(dataToSend);
                stream.Write(data, 0, data.Length);
                Debug.Log("Da gui map!");

                // Nhận phản hồi
                data = new byte[1024];
                int bytes = stream.Read(data, 0, data.Length);
                string responseData = Encoding.ASCII.GetString(data, 0, bytes);
                Debug.Log("C++ phan hoi: " + responseData);


            }

            stream.Close();
            client.Close();
        }
        catch (Exception e)
        {
            Debug.LogError("Loi ket noi: " + e.Message);
        }
    }

    public string GetAgentData() {
        StringBuilder sb = new StringBuilder();
        if (mapLoad != null && mapLoad.generatedEnemyFolder != null) {
            foreach (Transform enemyTransform in mapLoad.generatedEnemyFolder)
            {
                Vector3 pos = enemyTransform.position;

                // QUY ĐỔI NGƯỢC: Từ tọa độ Unity sang chỉ số (i, j) trong file text
                // j = x + 13
                // i = 13 - y - 1
                int j = Mathf.RoundToInt(pos.x + 13);
                int i = Mathf.RoundToInt(12 - pos.y);

                if (i >= 0 && i <= 26 && j >= 0 && j <= 26) sb.Append($"\n{j + i * 26}");
            }
        }
        return sb.ToString();
    }

    public string GetTaskData() {
        StringBuilder sb = new StringBuilder();
        Vector3 posPlayer1 = mapLoad.player1.position;
        int j1 = Mathf.RoundToInt(posPlayer1.x + 13);
        int i1 = Mathf.RoundToInt(12 - posPlayer1.y);
        sb.Append($"\n{"1"} {j1 + i1 * 26}");
        if (mapLoad.GetPlayer2()) {
            Vector3 posPlayer2 = mapLoad.player2.position;
            int j2 = Mathf.RoundToInt(posPlayer2.x + 13);
            int i2 = Mathf.RoundToInt(12 - posPlayer2.y);
            sb.Append($"\n{"2"} {j2 + i2 * 26}");
        }
        Vector3 posEagle = eagle.position;
        int j3 = Mathf.RoundToInt(posEagle.x + 13);
        int i3 = Mathf.RoundToInt(12 - posEagle.y);
        sb.Append($"\n{"3"} {j3 + i3 * 26}");
        return sb.ToString();
    }
}
